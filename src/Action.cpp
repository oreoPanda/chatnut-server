/*
 * Action.cpp
 *
 *  Created on: Feb 16, 2016
 *      Author: i_fly_gliders
 */

#include "Action.h"

namespace action
{

	Action::Action(networking::Client * owner)
	:owner(owner), type(WHO)
	{

	}

	Action::~Action()
	{

	}

	void Action::add_object(std::string const & name, std::forward_list<networking::Buddy>::iterator iter)
	{
		networking::Buddy tempbuddy(this->owner, name, iter);
		objects.push_front(tempbuddy);
	}

	/*networking::Buddy & Action::get_object_by_name(std::string name)	TODO use this
	{

	}*/

} /* namespace action */
