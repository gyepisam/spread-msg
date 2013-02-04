#include <sp.h>
#include <stdio.h>  /* sprintf */
#include <stdlib.h>
#include <string.h> /* strncmp */

#include "spread_msg.h"
#include "spread_error.h"

char *program;

#define MAX_GROUPS 1 

int group_action(mailbox mbox, int (*action)(mailbox, const char *)) {
    int i;
    int ret;
    char group_name[MAX_GROUP_NAME];

    for (i = 0; i < MAX_GROUPS; i++) {
        sprintf(group_name, "%s%d", "group", i);
        ret = action(mbox, group_name);
        if (ret < 0)
            spread_error(ret);
    }

    return MAX_GROUPS;
}

size_t count = 0;
size_t bytes = 0;
size_t seen = 0;


void print_totals() {
  print("seen: %ld, count: %ld, bytes: %ld\n", seen, count, bytes);
}

int main(int argc, char *argv[]) {
    mailbox mbox;
    char private_group[MAX_GROUP_NAME];
    int done = 0;
    int ret;
    sp_msg m;
    int joined;

    program = argv[0];

//    atexit(print_totals);

    ret = SP_connect("4803@localhost", "test_recv_msg", 1, 1, &mbox, &private_group[0]);
    if (ret < 0)
        spread_error(ret);

    joined = group_action(mbox, SP_join);

    sp_msg_init(&m);

    while (!done) {
        sp_recv_msg(mbox, &m, 0);

        switch(m.type) {
            case MSG_REG:
                bytes += m.u.reg.len;
                count = (unsigned long)m.u.reg.data;
                seen++;
                m.u.reg.data += sizeof(long); 
                m.u.reg.len -=  sizeof(long);
                m.u.reg.data[m.u.reg.len] = 0;

                if (strncmp(m.u.reg.data, "quit", 4) == 0)
                    group_action(mbox, SP_leave);
                break;

            case MSG_SELF_LEAVE:
                if (--joined == 0)
                    done = 1;
                break;

            case MSG_MEMB:
            case MSG_TRANS:
                 
                break;
            default:
                break; /* ignored */
        }
        print_msg(&m);
        print("\n");
    }
    sp_msg_reset(&m);

    return 0;
}

