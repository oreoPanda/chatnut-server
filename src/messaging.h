/*
 * Message.h
 *
 *  Created on: Feb 11, 2016
 *      Author: i_fly_gliders
 */

#ifndef MESSAGING_H_
#define MESSAGING_H_

#include <forward_list>
#include <vector>

#include "networking.h"
#include "Action.h"

namespace messaging
{

	/*enumerator type for replies to clients. Starts with 32 so nothing is messed up by '\n', '\0' or other special characters*/
	typedef enum
	{
		CONNECTED = 32,
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
	}reply;

	/*c-style strings to go along with the reply enumerator*/
	#define CONNECTED_STR "Connected\n"
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
	#define ERROR_STR "That command doesn't exist. Try `/help`\n."
	#define NOARG_STR "This command requires an argument.\n"
	#define NOMEM_STR "The server doesn't have enough memory left for the requested operation.\n"

	class Command
	{
	public:
		Command(std::string const str, networking::Client * const cur, std::forward_list<action::Action> * actionsptr);
		virtual ~Command();
		bool isCommand() const;
		void start_eval() const;
	private:
		bool iscmd;
		std::string command;
		std::vector<std::string> arguments;
		networking::Client * current;	//TODO should be a const pointer
		std::forward_list<action::Action> * actionlist;
		void who_handle() const;
		void unwho_handle() const;
		void lookup_handle() const;
		void login_handle() const;
		void logout_handle() const;
		void unknown_handle() const;
	};

	class Message
	{
	public:
		Message(std::string const & str, networking::Client * const cur);
		virtual ~Message();
		void send() const;
		void send_signal_connected();
	private:
		std::string message;
		networking::Client *cur;
	};

} /* namespace messaging */

#endif /* MESSAGING_H_ */
