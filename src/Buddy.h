/*Buddy.h*/

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

#ifndef SRC_BUDDY_H_
#define SRC_BUDDY_H_

#include "networking.h"

namespace networking
{

	class Client;

	/*Buddy class
	 * objects of this class are stored in two places, that is the buddylist and the reversebuddylist of each client
	 * in buddylist:
	 * 		client is a pointer to the buddy's client object
	 * 		clientname is the name of the buddy
	 * 		iter is an iterator of the reversebuddylist of the buddy
	 * in reversebuddylist of a client (client is here THIS):
	 * 		client is a pointer to the client object who has THIS as buddy
	 * 		clientname is empty
	 * 		iter is an iterator of the buddylist of client who has THIS as buddy*/
	class Buddy
	{
		public:
			Buddy();
			~Buddy();
			void set_client(Client * const cli);
			void set_name(std::string const & name);
			void set_iter(std::list<Buddy>::iterator const & i);
			Client * get_client();
			std::string const & get_name() const;
			std::list<Buddy>::iterator const & get_iter() const;
		private:
			Client * client;
			std::string clientname;
			std::list<Buddy>::iterator iter;
	};

}




#endif /* SRC_BUDDY_H_ */
