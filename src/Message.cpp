/*
 * Message.cpp
 *
 *  Created on: Feb 11, 2016
 *      Author: i_fly_gliders
 */

#include "messaging.h"

namespace messaging
{

	Message::Message(std::string const & str, networking::Client * const cur)
	:message(""), sender(cur)
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
	void Message::send() const
	{
		//iterator of the sender's buddylist
		std::list<networking::Buddy>::iterator buddyiter = sender->get_begin_buddy_iter();

		//send the message to each buddy
		while(buddyiter != sender->get_end_buddy_iter() )
		{
			if(buddyiter->get_client() )
			{
				//send message to buddy
				buddyiter->get_client()->Send(message);
			}
			else
			{
				//TODO store message on server
			}
			sender->advance_buddy_iter(buddyiter);
		}

		return;
	}

} /* namespace messaging */
