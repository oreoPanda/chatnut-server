/*
 * Message.cpp
 *
 *  Created on: Feb 11, 2016
 *      Author: i_fly_gliders
 */

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

        /*sends the message to all buddies of the sender*/
	void Message::send_or_store() const
	{
		//iterator of the sender's buddylist
		std::list<networking::Buddy>::iterator buddyiter = this->sender->get_begin_buddy_iter();

		//send the message to each buddy
		while(buddyiter != this->sender->get_end_buddy_iter() )
		{
			if(buddyiter->get_client() )
			{
				//send message to buddy
				buddyiter->get_client()->Send(this->message);
			}
			else
			{
				/*detach the sender's name from the message so that less disk space is used*/
				std::string pure_message;
				int begin = this->message.find_first_of(" ");
				pure_message = this->message.substr(begin);

				/*create a StorageWriter object and store the message on disk*/
				fileio::StorageWriter store(buddyiter->get_client()->get_Name(), this->sender->get_Name(), pure_message, this->logger);
				store.write();
			}
			this->sender->advance_buddy_iter(buddyiter);
		}

		return;
	}

} /* namespace messaging */
