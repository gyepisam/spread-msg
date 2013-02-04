/* DO NOT EDIT. This file is auto generated from ... */

#include <stdlib.h>
#include <sp.h>
#include "utils.h"

static struct {
	 int code;
	 char *desc;
       } errors[] = {
 { 0, "unknown error" },

{ ILLEGAL_SPREAD, "ILLEGAL_SPREAD - The spread_name given to connect to was illegal for some reason. Usually because it was a unix socket on Windows95/NT, an improper format for a host or an illegal port number" },
{ COULD_NOT_CONNECT, "COULD_NOT_CONNECT - Lower level socket calls failed to allow a connection to the specified spread daemon right now." },
{ CONNECTION_CLOSED, "CONNECTION_CLOSED - During communication to establish the connection errors occured and the setup could not be completed." },
{ REJECT_VERSION, "REJECT_VERSION - The daemon or library has a version mismatch." },
{ REJECT_NO_NAME, "REJECT_NO_NAME - This is a protocol error which should not occur if the connection request is sent correctly to the daemon. It means that the length of the user name was not sent (if no user name is provided a length of 0 must be sent)." },
{ REJECT_ILLEGAL_NAME, "REJECT_ILLEGAL_NAME - Name provided violated some requirement (length or used an illegal character)" },
{ REJECT_NOT_UNIQUE, "REJECT_NOT_UNIQUE - Name provided is not unique on this daemon. Recommended response is to try again with a different name." },
{ ILLEGAL_MESSAGE, "ILLEGAL_MESSAGE - The message had an illegal structure, like a scatter not filled out correctly. Or it is not a membership message." },
{ BUFFER_TOO_SHORT, "BUFFER_TOO_SHORT - The allocated array of member names can not hold the number of members in this VS set. No data is parsed. Reallocate a larger array of member names (the required number is available in the corresponding vs_set_info struct) and call this function again." },
{ BUFFER_TOO_SHORT, "BUFFER_TOO_SHORT - The allocated array of vs_set_info structs can not hold the number of VS sets contained in this membership message. No data is parsed. Reallocate a larger array of vs_set_info structs (the required number is available in the membership_info struct) and call this function again." },
{ ILLEGAL_GROUP, "ILLEGAL_GROUP - The group given to join was illegal for some reason. Usually because it was of length 0 or length > MAX_GROUP_NAME" },
{ ILLEGAL_SESSION, "ILLEGAL_SESSION - The session specified by mbox is illegal. Usually because it is not active." },
{ CONNECTION_CLOSED, "CONNECTION_CLOSED - During communication errors occured and the join could not be initiated." },
{ ILLEGAL_GROUP, "ILLEGAL_GROUP - The group given to leave was illegal for some reason. Usually because it was of length 0 or length > MAX_GROUP_NAME" },
{ ILLEGAL_SESSION, "ILLEGAL_SESSION - The session specified by mbox is illegal. Usually because it is not active." },
{ CONNECTION_CLOSED, "CONNECTION_CLOSED - During communication errors occured and the leave could not be initiated." },
{ ILLEGAL_SESSION, "ILLEGAL_SESSION - The mbox given to multicast on was illegal." },
{ ILLEGAL_MESSAGE, "ILLEGAL_MESSAGE - The message had an illegal structure, like a scatter not filled out correctly." },
{ CONNECTION_CLOSED, "CONNECTION_CLOSED - During communication to send the message errors occured and the send could not be completed." },
{ ILLEGAL_SESSION, "ILLEGAL_SESSION - The mbox given to multicast on was illegal." },
{ ILLEGAL_MESSAGE, "ILLEGAL_MESSAGE - The message had an illegal structure, like a scatter not filled out correctly." },
{ CONNECTION_CLOSED, "CONNECTION_CLOSED - During communication to send the message errors occured and the send could not be completed." },
{ ILLEGAL_SESSION, "ILLEGAL_SESSION - The mbox given to multicast on was illegal." },
{ ILLEGAL_MESSAGE, "ILLEGAL_MESSAGE - The message had an illegal structure, like a scatter not filled out correctly." },
{ CONNECTION_CLOSED, "CONNECTION_CLOSED - During communication to send the message errors occured and the send could not be completed." },
{ ILLEGAL_SESSION, "ILLEGAL_SESSION - The session specified by mbox is illegal. Usually because it is not active." },
{ ILLEGAL_SESSION, "ILLEGAL_SESSION - The mbox given to receive on was illegal." },
{ ILLEGAL_MESSAGE, "ILLEGAL_MESSAGE - The message had an illegal structure, like a scatter not filled out correctly." },
{ CONNECTION_CLOSED, "CONNECTION_CLOSED - During communication to receive the message communication errors occured and the receive could not be completed." },
{ GROUPS_TOO_SHORT, "GROUPS_TOO_SHORT - If the groups array is too short to hold the entire list of groups this message was sent to then this error is returned and the num_groups field will be set to the negative of the number of groups needed." },
{ BUFFER_TOO_SHORT, "BUFFER_TOO_SHORT - If the message body buffer mess is too short to hold the message being received then this error is returned and the endian_mismatch field is set to the negative value of the required buffer length." },
{ ILLEGAL_MESSAGE, "ILLEGAL_MESSAGE - The message had an illegal structure, like a scatter not filled out correctly. Or it is not a membership message." },
{ BUFFER_TOO_SHORT, "BUFFER_TOO_SHORT - The allocated array of member names can not hold the number of members in this VS set. No data is parsed. Reallocate a larger array of member names (the required number is available in the corresponding vs_set_info struct) and call this function again." },
{ BUFFER_TOO_SHORT, "BUFFER_TOO_SHORT - The allocated array of vs_set_info structs can not hold the number of VS sets contained in this membership message. No data is parsed. Reallocate a larger array of vs_set_info structs (the required number is available in the membership_info struct) and call this function again." },
{ ILLEGAL_SESSION, "ILLEGAL_SESSION - The mbox given to multicast on was illegal." },
{ ILLEGAL_MESSAGE, "ILLEGAL_MESSAGE - The message had an illegal structure, like a scatter not filled out correctly." },
{ CONNECTION_CLOSED, "CONNECTION_CLOSED - During communication to send the message errors occured and the send could not be completed." },
{ ILLEGAL_SESSION, "ILLEGAL_SESSION - The mbox given to receive on was illegal." },
{ ILLEGAL_MESSAGE, "ILLEGAL_MESSAGE - The message had an illegal structure, like a scatter not filled out correctly." },
{ CONNECTION_CLOSED, "CONNECTION_CLOSED - During communication to receive the message communication errors occured and the receive could not be completed." },
{ GROUPS_TOO_SHORT, "GROUPS_TOO_SHORT - If the groups array is too short to hold the entire list of groups this message was sent to then this error is returned and the num_groups field will be set to the negative of the number of groups needed." },
{ BUFFER_TOO_SHORT, "BUFFER_TOO_SHORT - If the message body buffer mess is too short to hold the message being received then this error is returned and the endian_mismatch field is set to the negative value of the required buffer length." },

};

char *spread_error_desc(int code) {
     int i;
     for (i = 0; i < 42 ; i++)
     	 if (errors[i].code == code)
	    return errors[i].desc;

     return errors[0].desc;
}

void warn_spread_error(int code) {
     warn("%s", spread_error_desc(code));
}

void spread_error(int code) {
     warn_spread_error(code);
     abort();
}
