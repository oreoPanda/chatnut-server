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
        :message(str), sender(cur)
	{

	}

	Message::~Message()
	{

	}

        /*sends the message to all buddies of the sender*/
	void Message::send() const
	{
                //iterator of the sender's buddylist
                std::forward_list<networking::Buddy>::iterator buddyiter = sender->get_begin_buddy_iter();

                //send the message to each buddy
                while(buddyiter != sender->get_end_buddy_iter() )
                {
                    buddyiter->get_client()->Send(message);
                    sender->advance_buddy_iter(buddyiter);
                }
                
		return;
	}

} /* namespace messaging */
