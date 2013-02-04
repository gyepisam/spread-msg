#include <sp.h>
#include "spread_msg.h"

static void print_group_id(group_id *grp_id) {
    if (grp_id){
        print(" (gid %d %d %d)\n", grp_id->id[0], grp_id->id[1], grp_id->id[2]);
    }
}

static void print_groups(char (*groups)[MAX_GROUP_NAME], int num_groups) {
    if (num_groups == 0) {
        print("Groups: empty\n");
    } 
    else {
        int i;
        print("Groups: (count %d)\n  ", num_groups);
        for (i = 0; i < num_groups; groups++, i++ ) {
            print("%s%s", i > 0 ? ", " : "", groups);
        }
        print("\n");
    }
}

void print_regular_msg(sp_msg_reg *m) {
    print("  (endian_mismatch %d)\n", m->endian_mismatch);
    print_groups(m->groups, m->num_groups);
    print("  (data %d %*1$s)\n", m->len, m->data);
}

void print_membership_msg(sp_msg_memb *m) {
    int i;

    print_group_id(m->gid);

    if (m->changed_member != 0)
        print(" (changed %s)\n", m->changed_member);

    print_groups(m->groups, m->num_groups);

    if (m->num_vs_sets == 0) {
        print("VS sets: empty\n");
    }
    else {
        vs_set_info *p = m->vs_sets;
        int j;

        print("VS sets: (count %d)\n  ", m->num_vs_sets);
        for(i = 0; i < m->num_vs_sets; i++, p++) {
            print("%s%c", i > 0 ? ", " : "", i == m->my_vs_set_index ? '<' : '[');
            for (j = 0; j < p->num_members; j++) {
                print("%s%s", j > 0 ? ", " : "", m->vs_members[p->members_offset + j]);
            }
            print("%c", i == m->my_vs_set_index ? '>' : ']');
        }
        print("\n");
    }
}

void print_unknown_msg(sp_msg_unk *m) {
    print(" (message type %d)\n", m->type);
    if (m->len != 0) {
        print(" (data %d %*1$s)\n", m->len, m->data);
    }
}

void print_msg(sp_msg *m) {

    print("%s message: service_type = %08X sender=%s\n", msg_type_str(m->type), m->service_type, m->sender);

    switch(m->type) {
        case MSG_UNK:
            print_unknown_msg(&m->u.unk);
            break;
        case MSG_REG:
            print_regular_msg(&m->u.reg);
            break;
        case MSG_TRANS:
            print_group_id(m->u.trans.gid);
            break;
        case MSG_MEMB:
            print_membership_msg(&m->u.memb);
            break;
        case MSG_SELF_LEAVE:
            break;
        default:
            print("unknown message type\n");
    }
}
