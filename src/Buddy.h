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

	class Buddy
	{
		public:
			Buddy(std::string const & name);
			~Buddy();
			void set(Client const * const cli, std::forward_list<Buddy>::iterator const & i);
			std::string const & get_name() const;
		private:
			Client const * client;
			std::string const clientname;
			std::forward_list<Buddy>::iterator iter;
	};

}




#endif /* SRC_BUDDY_H_ */
