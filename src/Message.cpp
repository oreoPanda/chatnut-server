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
	:message(str), cur(cur)
	{

	}

	Message::~Message()
	{

	}

	void Message::send() const
	{
		return;
	}

	void Message::send_signal_connected()
	{
		/*set up the message TODO check that it has the right length*/
		std::string str = CONNECTED + CONNECTED_STR;

		/*send the message*/
		cur->Send(str);
	}

} /* namespace messaging */
