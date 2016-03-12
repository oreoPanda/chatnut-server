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

#include "LogWriter.h"
#include "networking.h"
#include "Storage.h"
#include "Userlist.h"

namespace messaging
{

	/*enumerator type for replies to clients. Starts with 32 so nothing is messed up by '\n', '\0' or other special characters*/
	typedef enum
	{
		CONNECTED = 32,
		HELP,
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
		NOARG,		//TODO could use a TOO_MANY_ARGS or WRONG_NUM_OF_ARGS instead bzw. as well
		NOMEM,
		ERROR,
		COUNT
	}reply;

	class Command
	{
	public:
		Command(networking::Client * const cur, fileio::Userlist & list, fileio::LogWriter & logger);
		Command(std::string const str, networking::Client * const cur, std::forward_list<networking::Action> * actionsptr, fileio::Userlist & list, fileio::LogWriter & logger);
		~Command();

		bool isCommand() const;
		void evaluate() const;
	private:
		bool iscmd;
		std::vector<std::string> replies;		//TODO array? const?
		networking::Client * const current;
		std::string const command;
		std::vector<std::string> arguments;	//TODO maybe change to array, maybe const?
		std::forward_list<networking::Action> * actionlist;
		fileio::Userlist & userlist;
		fileio::LogWriter & logger;

		void init_replies();
		void connected_handle() const;
		void who_handle() const;
		void unwho_handle() const;
		void lookup_handle() const;
		void login_handle() const;
		void logout_handle() const;
		void unknown_handle() const;
		void construct_reply(reply const indic, std::string const data = "") const;
	};

	class Message
	{
	public:
		Message(std::string const & str, networking::Client * const cur, fileio::LogWriter & logger);
		virtual ~Message();
		void send_or_store() const;
	private:
		std::string message;
		networking::Client * const sender;
		fileio::LogWriter & logger;
	};

} /* namespace messaging */

#endif /* MESSAGING_H_ */
