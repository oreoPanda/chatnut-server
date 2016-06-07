/*Action.cpp*/

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

#include "networking.h"

namespace networking    //TODO put back into namespace action
{

	Action::Action(Client * const creator, std::string const & recv_name, std::list<Buddy>::iterator const & i)
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
	std::list<Buddy>::iterator const & Action::get_buddy_iter() const
	{
		return this->iter;
	}

} /* namespace networking */
