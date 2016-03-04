/*
 * Action.cpp
 *
 *  Created on: Feb 16, 2016
 *      Author: i_fly_gliders
 */

#include "networking.h"

namespace networking    //TODO put back into namespace action
{

	Action::Action(Client * const creator, std::string const & recv_name, std::forward_list<Buddy>::iterator const & i)
        :creator(creator), receivername(recv_name), iter(i)
	{

	}

	Action::~Action()
	{

	}
	
	/*returns a pointer to the creator of the action*/
	Client * Action::get_creator()
        {
            return this->creator;
        }

        /*returns the receiver's name*/
	std::string const & Action::get_receiver() const
	{
		return this->receivername;
	}

	/*returns the iterator of owner's buddylist*/
	std::forward_list<Buddy>::iterator const & Action::get_buddy_iter() const
	{
		return this->iter;
	}

} /* namespace networking */
