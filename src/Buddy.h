/*
 * Buddy.h
 *
 *  Created on: Feb 16, 2016
 *      Author: i_fly_gliders
 */

#ifndef SRC_BUDDY_H_
#define SRC_BUDDY_H_

#include <forward_list>

#include "networking.h"

namespace networking
{

	class Client;

	/*Buddy class
	 * objects of this class are stored in two places, that is the buddylist and the reversebuddylist of each client
	 * in buddylist:
	 * 		client is a pointer to the buddy's client object
	 * 		clientname is the name of the buddy
	 * 		iter stays empty
	 * in reversebuddylist of a client (here THIS):
	 * 		client is a pointer to the client object who has THIS as buddy
	 * 		clientname is empty
	 * 		iter is an iterator of the buddy's buddylist*/
	class Buddy
	{
		public:
			Buddy();
			~Buddy();
			void set(Client * const cli);
			void set(std::string const & name);
			void set(std::forward_list<Buddy>::iterator const & i);
			Client * get_client();
			std::string const & get_name() const;
			std::forward_list<Buddy>::iterator const & get_iter() const;
		private:
			Client * client;
			std::string clientname;
			std::forward_list<Buddy>::iterator iter;
	};

}




#endif /* SRC_BUDDY_H_ */
