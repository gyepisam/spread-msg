#!/usr/bin/python

import os
import os.path
import random
import spread
import string
import struct
import thread

def run(group, messages):
    m = spread.connect('4803@localhost','_%s' % (os.getpid(),), 1, 1)
    m.join(group)
    count = 0
    while True:
        count += 1
        message = struct.pack('L', count) + random.choice(messages)
        message_type = random.randint(0, len(messages) % 10)
        try:
            m.multicast(spread.SAFE_MESS, group, message, message_type)
        except:
            pass
        msg = m.receive()
        if type(msg) is spread.MembershipMsgType:
            print "Membership message:"
            print "  members (%s) group (%s) extra(%s)" % \
                    (msg.members, msg.group, msg.extra)
        else:
            print "Regular Message:"
            print "  from (%s), to groups (%s) message (type=%d len=%d content=%s)" % \
                  (msg.sender, msg.groups, msg.msg_type, len(msg.message), msg.message)

            if 'quit' in msg.message:
                break

    m.leave(group)

if __name__ == '__main__':
    import sys
   
    group = 'group0'

    text_path = None
    optind = None 
    skip = None
    data_char = 'X'

    argv = sys.argv

    for i in range(len(argv[1:])):
        i += 1
        a = argv[i]
        if skip is not None:
            skip -= 1
            if skip == 0:
                skip = None
            continue

        if a in ('--group', '-g'):
            group = int(argv[i + 1])
            skip = 1
        elif a in ('--data', '-d'):
            text_path = argv[i + 1]
            skip = 1
        elif a in ('--char', '-c'):
            data_char = argv[i+1]
            skip = 1
        else:
            optind = i
            break

    if optind and not text_path :
        try:
            text_path = argv[optind]
        except IndexError:
            pass

    if text_path is None:
        data = [data_char * i for i in range(100)]
    else:
        data = map(string.strip, file(os.path.expanduser(text_path)).readlines())

    run(group, data)
