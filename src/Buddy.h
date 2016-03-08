/*
 * Buddy.h
 *
 *  Created on: Feb 16, 2016
 *      Author: i_fly_gliders
 */

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
