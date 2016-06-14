/*Message.h*/

/*Copyright (C) 2016 Jonas Fuglsang-Petersen*/

/*This file is part of chatnut-server.*/

/*chatnut-server is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

chatnut-server is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with chatnut.  If not, see <http://www.gnu.org/licenses/>.*/

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
		void registration_handle() const;
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
