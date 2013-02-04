#include <sp.h>
#include <string.h>
#include <assert.h>

#include "spread_msg.h"
#include "spread_error.h"

#define fatal_spread_error(code) fatal("%s: %d - %s\n", __FILE__, __LINE__, spread_error_desc(code))

static struct { sp_msg_type code; char *name; } msg_types[] =
  {
    { MSG_UNK, "Unknown" },
    { MSG_REG, "Regular" },
    { MSG_TRANS, "Transitional" },
    { MSG_MEMB, "Membership" },
    { MSG_SELF_LEAVE, "Self Leave" },
  };

const char *msg_type_str(sp_msg_type type) {
  int i;

  for (i = 0; i < nelem(msg_types); i++)
    if (type == msg_types[i].code)
      return msg_types[i].name;

  return msg_types[0].name;
}

sp_msg *sp_msg_new() {
    sp_msg *m;
   
    m = xmalloc(sizeof(sp_msg));

    sp_msg_init(m);

    return m;
}

void resize_msg_group(sp_msg *m, int n) {
    m->groups_cap = n;
    m->groups = (char (*)[MAX_GROUP_NAME]) xmalloc(n * MAX_GROUP_NAME);
}

void resize_msg_data(sp_msg *m, int n) {
    m->data_cap = n;
    m->data = (char *) xmalloc(n);
}

group_id *copy_msg_gid(sp_msg *m, const void *g) {
    memcpy(&m->gid, g, sizeof(group_id));
    return &m->gid;
}

char * copy_msg_changed_member(sp_msg *m, char *c) {
    memcpy(m->changed_member, c, MAX_GROUP_NAME);
    return m->changed_member;
}

void sp_msg_init(sp_msg *m) {

    m->type = MSG_UNK;

    m->service_type = 0;
    m->sender = (char *)xmalloc(MAX_GROUP_NAME);
    resize_msg_group(m, INITIAL_NUM_GROUP_MEMBERS);
    resize_msg_data(m, INITIAL_DATA_LEN);
    m->changed_member = (char *)xmalloc(MAX_GROUP_NAME);
    m->vs_sets = 0;
    m->vs_sets_cap = 0;
    m->vs_members = 0;
    m->vs_members_cap = 0;

    m->u.reg.type = 0;
    m->u.reg.endian_mismatch = 0;
    m->u.reg.groups = 0;
    m->u.reg.num_groups = 0;
    m->u.reg.data = m->data;
    m->u.reg.len = 0;

    m->u.trans.service_type = 0;
    m->u.trans.sender = 0;
    m->u.trans.gid = 0;

    m->u.memb.service_type = 0;
    m->u.memb.sender = 0;
    m->u.memb.gid = &m->gid;
    m->u.memb.groups = 0;
    m->u.memb.num_groups = 0;
    m->u.memb.my_group_index = 0;
    m->u.memb.changed_member = 0;
    m->u.memb.vs_sets = 0;
    m->u.memb.num_vs_sets = 0;
    m->u.memb.my_vs_set_index = 0;
    m->u.memb.vs_members = 0;
    m->u.memb.num_vs_members = 0;
    m->u.memb.vs_sets_cap = 0;
    m->u.memb.vs_members_cap = 0;

    m->u.sl.service_type = 0;
    m->u.sl.sender = 0;

    m->u.unk.service_type = 0;
    m->u.unk.sender = 0;
    m->u.unk.type = 0;
    m->u.unk.data = 0;
    m->u.unk.len = 0;
}

void sp_msg_free(sp_msg *m) {
    sp_msg_reset(m);
    xfree(m);
}

void sp_msg_reset(sp_msg *m) {

    xfree(m->sender);
    xfree(m->groups);
    xfree(m->data);
    xfree(m->changed_member);

    if (m->vs_sets != nil){
        xfree(m->vs_sets);
        m->vs_sets_cap = 0;
    }

    if (m->vs_members != nil){
        xfree(m->vs_members);
        m->vs_members_cap = 0;
    }
}

/* vs_sets list group members. 
   untangle sets of vs_sets, packed into data.
*/
static void parse_vs_sets(const char *data, int num_vs_sets, sp_msg *msg) {

  int i;
  int ret;
  int num_vs_members = 0;
  vs_set_info *p;
  int offset;
  sp_msg_memb *m = &msg->u.memb;

  m->num_vs_sets = num_vs_sets;

  if (m->num_vs_sets == 0)
    return;

  if (msg->vs_sets_cap < m->num_vs_sets) {
    msg->vs_sets_cap = m->num_vs_sets;
    msg->vs_sets = (vs_set_info *) xrealloc(msg->vs_sets, m->num_vs_sets * sizeof(vs_set_info));
  }

  m->vs_sets = msg->vs_sets;
  ret = SP_get_vs_sets_info(data, m->vs_sets, m->num_vs_sets, &m->my_vs_set_index);
  if (ret < 0)
    fatal_spread_error(ret);

  /* count all vs set members */
  for (p = m->vs_sets, num_vs_members = 0, i = 0; i < m->num_vs_sets; i++, p++)
    num_vs_members += p->num_members;

  /* allocate a chunk big enough for everyone */
  if (msg->vs_members_cap < num_vs_members) {
    msg->vs_members_cap = num_vs_members;
    msg->vs_members = (char (*)[MAX_GROUP_NAME]) xrealloc(msg->vs_members, num_vs_members * MAX_GROUP_NAME);
  }
  m->num_vs_members = num_vs_members;
  m->vs_members = msg->vs_members;
  
  /* Assign each chunk into place based on count and offset */
  for (p = m->vs_sets, i = 0, offset = 0; i < m->num_vs_sets; i++, p++) {

    ret = SP_get_vs_set_members(data, p, &m->vs_members[offset], p->num_members);

    if (ret < 0)
      fatal_spread_error(ret);

    /* change offsets to reflect position in array, not buffer */
    p->members_offset = offset;
    offset +=  p->num_members * MAX_GROUP_NAME;
  }
}

int sp_recv_msg(mailbox mbox, sp_msg *m, int drop_recv) {

  int ret;

  service service_type;
  int num_groups;
  short message_type;
  int endian_mismatch;
  membership_info memb_info;


 REDO:

  service_type = drop_recv ? DROP_RECV : 0;
  num_groups = 0;
  endian_mismatch = 0;

  /* return value is either a length or an error code */
  ret = SP_receive(mbox, &service_type,
		   m->sender,
		   m->groups_cap, &num_groups, m->groups,
		   &message_type,
		   &endian_mismatch,
		   m->data_cap,
		   m->data);


  if (ret == BUFFER_TOO_SHORT) {
    resize_msg_data(m, -endian_mismatch);
    goto REDO;
  }
  else if (ret == GROUPS_TOO_SHORT) {
      resize_msg_group(m, -num_groups);
      goto REDO;
  }
  else if (ret < 0)
    fatal_spread_error(ret);
    
  if (Is_regular_mess(service_type)) {

    m->type = MSG_REG;
    m->u.reg.service_type = m->service_type = service_type;
    m->u.reg.sender = m->sender; /* same as private group name */
    m->u.reg.type = message_type;
    m->u.reg.endian_mismatch = endian_mismatch;
    m->u.reg.groups = m->groups;
    m->u.reg.num_groups = num_groups;
    m->u.reg.data = m->data;
    m->u.reg.len = ret; /* overloaded */
  }
  else if (Is_membership_mess(service_type)) {
    if (Is_transition_mess(service_type)) {

      m->type = MSG_TRANS;
      m->u.trans.service_type = m->service_type = service_type;
      m->u.trans.sender = m->sender;
      m->u.trans.gid = copy_msg_gid(m, m->data);

    }
    else if (Is_reg_memb_mess(service_type)) {

        m->type = MSG_MEMB;
        m->u.memb.service_type = m->service_type = service_type;
        m->u.memb.sender = m->sender;
        m->u.memb.groups = m->groups;
        m->u.memb.num_groups = num_groups;
        m->u.memb.my_group_index = message_type; /* overloaded */

        ret = SP_get_memb_info(m->data, service_type, &memb_info);
        if (ret < 0)
            fatal_spread_error(ret);

        m->u.memb.gid = copy_msg_gid(m, &memb_info.gid);


        if (Is_caused_join_mess(service_type) ||
            Is_caused_leave_mess(service_type) ||
            Is_caused_disconnect_mess(service_type))
            m->u.memb.changed_member = copy_msg_changed_member(m, memb_info.changed_member);

        parse_vs_sets(m->data, memb_info.num_vs_sets, m);

    }
    else if (Is_self_leave(service_type)) {
      m->type = MSG_SELF_LEAVE;

      m->u.sl.service_type = m->service_type = service_type;
      m->u.sl.sender = m->sender;
      m->u.sl.data = m->data;
      m->u.sl.len = ret;

    }
    else {
      m->type = MSG_UNK;

      m->u.unk.service_type = m->service_type = service_type;
      m->u.unk.sender = m->sender;
      m->u.unk.type = message_type;
      m->u.unk.data = m->data;
      m->u.unk.len = ret;

    }
  }

  return ret; 
}


