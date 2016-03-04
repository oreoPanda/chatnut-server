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
	:message(str), current(cur)
	{

	}

	Message::~Message()
	{

	}

	void Message::send() const
	{
		return;
	}

} /* namespace messaging */
