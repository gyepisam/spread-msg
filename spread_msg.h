#ifndef SPREAD_MSG_H
#define SPREAD_MSG_H

#include <sp.h>
#include "utils.h"

/* 
   spread_message adds a layer of abstracts and encapsulates the various
   kinds of messages SP_receive produces so application authors can focus
   on using spread sooner
*/


typedef enum { MSG_UNK = 1, MSG_REG, MSG_TRANS, MSG_MEMB, MSG_SELF_LEAVE } sp_msg_type;

const char *msg_type_str(sp_msg_type);

/* Specify initial data sizes.
 * The buffers will be enlarged dynamically.
 */

#define INITIAL_DATA_LEN 2048
#define INITIAL_NUM_GROUP_MEMBERS 15


/* a regular message. will contain application specific data */
typedef struct sp_msg_reg {
  service service_type;  /* don't move */
  char *sender;          /* don't move */
  
  
  char (*groups)[MAX_GROUP_NAME];
  int num_groups;

  int16 type;           /* application specific type */
  char *data;
  int len;

  int endian_mismatch;  /* flags if the data source has differing endianness */

} sp_msg_reg;

/* a transition message */
typedef struct {
  service service_type;  /* don't move */
  char *sender;          /* don't move */
  group_id *gid;
} sp_msg_trans;

/* heavy weight membership message */
typedef struct {
  service service_type;  /* don't move */
  char *sender;   /* don't move */

  group_id *gid;

  char (*groups)[MAX_GROUP_NAME];
  int num_groups;
  int my_group_index; /* index of this member in groups */

  char *changed_member; /* when a single host joins, leaves or is disconnected */

  /* 
     An array of vs_set_info structures, one for each vs_set that came together in a network event.
     Though reusing the data structure from spread, the offsets are relative to the vs_members array,
     not the message structure.
  */
  vs_set_info *vs_sets;
  int num_vs_sets;
  unsigned int my_vs_set_index;   /* Index of this member's vs_set in vs_set_info */
  size_t vs_sets_cap; /* private */

  /* 
     all members of all vs_sets.
     vs_sets offsets and counts into this array.
  */
  char (*vs_members)[MAX_GROUP_NAME];
  int  num_vs_members;
  size_t vs_members_cap; /* private */

} sp_msg_memb;


/* light weight membership message for self leave */
typedef struct {
  service service_type;  /* don't move */
  char *sender;   /* don't move */
  char *data;
  int len;
} sp_msg_sl;

/* unknown message. messages are initialized to this type
 * and should change into one of the other type if all goes well.
 * seeing this message 'in the wild' generally signifies an error
 * in the spread machinery.
 */
typedef struct {
  service service_type; /* don't move */
  char *sender;         /* don't move */
  int16 type;
  char *data;
  int len;
} sp_msg_unk;

/* generic message container */
typedef struct {
  service service_type;  /* don't move */
  char *sender;          /* don't move */
  sp_msg_type type;
  union {
    sp_msg_reg reg;
    sp_msg_trans trans;
    sp_msg_memb memb;
    sp_msg_unk unk;
    sp_msg_sl sl;
  } u;
  

  /* all private */
  int sender_cap;
  char (*groups)[MAX_GROUP_NAME];
  int groups_cap;

  char *data;
  int data_cap;

  char *changed_member;
  group_id gid;

  vs_set_info *vs_sets;
  size_t vs_sets_cap;

  char (*vs_members)[MAX_GROUP_NAME];
  size_t vs_members_cap;

} sp_msg;


sp_msg *sp_msg_new();
void sp_msg_free(sp_msg *m);

void sp_msg_init(sp_msg *);
void sp_msg_reset(sp_msg *);

/* prints message data to stderr */
void print_msg(sp_msg *m);
void print_regular_msg(sp_msg_reg *m);
void print_transition_msg(sp_msg_trans *m);
void print_membership_msg(sp_msg_memb *m);
void print_unknown_msg(sp_msg_unk *m);

/* wrapper arround SP_receive.
 *
   This call should be preceeded by a call to

        SP_connect(...);

   and as many calls to 

        SP_join(...)

   as necessary.


   mbox is a spread mailbox, as returned by SP_connect.
   drop_recv is a boolean (0|1) value denoting whether
   to enable DROP_RECV in the SP_receive call.
   
   msg is a pointer to an sp_msg_init(ialized) sp_msg structure.

   in order to reduce data copying, the structure is reused for each call.
*/
int sp_recv_msg(mailbox mbox, sp_msg *m, int drop_recv);

#endif
