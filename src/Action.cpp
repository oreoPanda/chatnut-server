/*
 * Action.cpp
 *
 *  Created on: Feb 16, 2016
 *      Author: i_fly_gliders
 */

#include "networking.h"

namespace networking    //TODO put back into namespace action
{

	Action::Action(std::string const & receivername, Client * const owner, std::forward_list<Buddy>::iterator const & i)
        :actionreceiver(receivername), owner(owner), iter(i)
	{

	}

	Action::~Action()
	{

	}

	std::string const & Action::get_receiver() const
	{
		return this->actionreceiver;
	}

        Client * Action::get_owner()
        {
            return this->owner;
        }

	/*returns the iterator that points at the action's creator's buddylist*/
	std::forward_list<Buddy>::iterator const & Action::get_buddy_iter() const
	{
		return this->iter;
	}

	/*adds an object to the list with the given data plus client value of Buddy is set to this->owner*/
	/*void Action::add_object(std::string const & name, std::forward_list<Buddy>::iterator const & iter)
	{
		Buddy tempbuddy(name);
		tempbuddy.set(this->owner, iter);
		objects.push_front(tempbuddy);
	}*/

	/*pop a buddy object from list, set name of b to this->owner->get_name and copy the rest of the data from the popped object to b*/
	/*returns false if no object left to pop, otherwise returns true*/
	/*bool pop_object_by_name(std::string const & name, Buddy & b)
	{
		std::forward_list<Buddy>::iterator i;

		i = objects.begin();
		while( iter != objects.end() )
		{
			if(i->get_name().compare(name) != 0)
			{
				Buddy temp(this->owner->get_name() );
				temp.set(i->)
				b = temp;
			}
			else
			{
				i++;
			}
		}
		return false;
	}*/

} /* namespace networking */
