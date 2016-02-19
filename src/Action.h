/*
 * Action.h
 *
 *  Created on: Feb 16, 2016
 *      Author: i_fly_gliders
 */

#ifndef SRC_ACTION_H_
#define SRC_ACTION_H_

#include <forward_list>

#include "networking.h"

namespace action
{
	/*typedef enum
	{
		WHO
	}actiontype;*/

	class Action
	{
	public:
		Action(networking::Client const * const owner);
		~Action();
		void add_object(std::string const & name, std::forward_list<networking::Buddy>::iterator const & iter);
		networking::Buddy get_object_by_name(std::string const & name);
	private:
		/*actiontype type;*/
		networking::Client const * const owner;
		std::forward_list<networking::Buddy> objects;
	};

} /* namespace messaging */

#endif /* SRC_ACTION_H_ */
