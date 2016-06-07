/*Message.cpp*/

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

#include "messaging.h"

namespace messaging
{

	Message::Message(std::string const & str, networking::Client * const cur, fileio::LogWriter & logger)
	:message(""), sender(cur), logger(logger)
	{
		/*construct message*/
		char * const indicator_c = static_cast<char * const>(calloc(1, sizeof(char) ) );
		indicator_c[0] = MESSAGE;
		std::string const indicator = indicator_c;
		message = indicator + str + '\n';
	}

	Message::~Message()
	{

	}

	/*sends the message to all buddies of the sender or stores it on server*/
	void Message::send_or_store() const
	{
		//iterator of the sender's buddylist
		std::list<networking::Buddy>::iterator buddyiter = this->sender->get_begin_buddy_iter();

		//go through all buddies
		while(buddyiter != this->sender->get_end_buddy_iter() )
		{
			//send message to each buddy
			if(buddyiter->get_client() )
			{
				buddyiter->get_client()->Send(this->message);
			}
			//store the message on server
			else
			{
				/*detach the sender's name from the message so that less disk space is used*/
				std::string pure_message;
				int begin = this->message.find_first_of(" ") + 1;
				int end = this->message.find_first_of("\n");
				pure_message = this->message.substr(begin, end-begin);

				/*create a StorageWriter object and store the message on disk*/
				fileio::StorageWriter store(buddyiter->get_name(), this->sender->get_Name(), pure_message, this->logger);
				store.write();
			}
			this->sender->advance_buddy_iter(buddyiter);
		}

		return;
	}

} /* namespace messaging */
