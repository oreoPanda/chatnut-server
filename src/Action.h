/*
 * Action.h
 *
 *  Created on: Feb 21, 2016
 *      Author: i_fly_gliders
 */

#ifndef SRC_ACTION_H_
#define SRC_ACTION_H_

#include <forward_list>

namespace networking
{
	class Buddy;
}

namespace action
{

	/*typedef enum
	{
		WHO
	}actiontype;*/

	/*mostly a storage class, stores an iterator to a buddy inside buddylist of a client that sent a /who command.
	 * actionreceiver is the name of the client that was added by the /who, the respective client will add the iterator
	 * from this action into his reverselist and set a pointer to himself and an iterator to the richt place in his reverselist
	 * in the element of the action's original creator buddylist*/
	/*Action currently only supports one iterator, it is wanted to expand supporting multiple iterators,
	 * each one set by a different /who request from a different client but to the same receiver. The receiver will
	 * only need to add all of the iterators in Action to his reverselist and edit all of the sender's buddylists as described above.
	 * The sender isn't stored because the iterator points directly at a spot in his buddylist.*/
	class Action
	{
	public:
		Action(std::string const & name, std::forward_list<networking::Buddy>::iterator const & i);
		~Action();
		std::string const & get_receiver() const;
		std::forward_list<networking::Buddy>::iterator const & get_buddy_iter() const;
		//void add_object(std::string const & name, std::forward_list<networking::Buddy>::iterator const & iter);
		//bool pop_object_by_name(std::string const & name, networking::Buddy & b);
	private:
		/*actiontype type;*/
		std::string const actionreceiver;
		std::forward_list<networking::Buddy>::iterator const iter;
	};

} /* namespace action */

#endif /* SRC_ACTION_H_ */
