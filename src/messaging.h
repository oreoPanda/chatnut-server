/*commands.h*/

/*  used by eval_cmd to tell cmd_reply what command was used	*
 *  and by the client to tell if name and buddies are set	    */

/* commands are:
 * /help
 * /list
 * /name
 * /who
 */

#define CONN_MSG_LEN 10

#define MSG_LEN 2000
#define CMD_LEN 120
#define NAMELEN 21			//last byte is null-pointer, so effective namelength is 20 bytes/letters
#define PASSLEN NAMELEN

#define TRUE 1
#define FALSE 0

//error codes
#define FILE_OPEN_ERROR -1

enum commands
{
	HELP_WANTED = 1,
	LIST,
	NAME_IS_SET,
	BUDDY_IS_SET,
	BUDDY_NOT_EXIST,
	LOGIN_SUCCESS,
	LOGIN_FAILURE,
        REGISTRATION_SUCCESS,
        REGISTRATION_FAILURE,
	NOARG,
	ERROR
};
