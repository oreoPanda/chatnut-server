/*commands.h*/

/*  used by eval_cmd to tell cmd_reply what command was used	*
 *  and by the client to tell if name and buddies are set	    */

/* commands are:
 * /help
 * /list
 * /name
 * /who
 */

#ifndef MESSAGING_H
#define MESSAGING_H

#define CONN_MSG_LEN 11

#define MSG_LEN 2000
#define CMD_LEN 120
#define NAMELEN 21			//last byte is null-pointer, so effective namelength is 20 bytes/letters
#define PASSLEN NAMELEN

//error codes
#define FILE_OPEN_ERROR -1

typedef enum
{
	CONNECTED = 48,
	HELP,
	LIST,
	BUDDY_IS_SET,
	BUDDY_IS_UNSET,
	BUDDY_NOT_EXIST,
	LOGIN_SUCCESS,
	LOGIN_FAILURE,
	REGISTRATION_SUCCESS,
	REGISTRATION_FAILURE,
	LOOKUP_SUCCESS,
	LOOKUP_FAILURE,
	MESSAGE,
	NOARG,
	NOMEM,
	ERROR
}commandreply;

/*commandreply strings terminated with '\n\0'*/
#define ERROR_STR "That command doesn't exist. Try `/help`\n."
#define HELP_STR "Usage: [command] [space-separated arguments]\n\
        Commands:\n\
            \t/help\tshow this help\n\
            \t/name\tset your name\n\
            \t/who\tset buddies you want to talk to\n\
            \t/list\tshow a list of connected people\n\
        Arguments:\n\
            \tnames\n"
#define BUDDY_IS_SET_STR "Successfully set buddy.\n"
#define BUDDY_IS_UNSET_STR "Successfully unset buddy.\n"
#define BUDDY_NOT_EXIST_STR "The buddy you requested does not exist.\n"
#define LOGIN_SUCCESS_STR "Login successful.\n"
#define LOGIN_FAILURE_STR "Login unsuccessful. Please try again.\n"
#define REGISTRATION_SUCCESS_STR "Registration successful.\n"
#define REGISTRATION_FAILURE_STR "Registration unsuccessful. Please try again.\n"
#define LOOKUP_SUCCESS_STR "User found.\n"
#define LOOKUP_FAILURE_STR "That user doesn't exist. Please try again.\n"
#define NOARG_STR "This command requires an argument.\n"
#define NOMEM_STR "The server doesn't have enough memory left for the requested operation.\n"

#endif	//MESSAGING_H
