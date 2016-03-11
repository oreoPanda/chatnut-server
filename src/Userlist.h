/*
 * Userlist.h
 *
 *  Created on: Mar 11, 2016
 *      Author: i_fly_gliders
 */

#ifndef SRC_USERLIST_H_
#define SRC_USERLIST_H_

/*virtual: method will be redefined in class that inherits
 * calling methods in Base classes if they were overwritten:
 * 		[obj]->[Baseclassname]::[method]
 * if a destructor is not virtual than only the base destructor is called
 * 		memory leak
 * if a destructor is virtual, then first the last one in inheritance is called,
 * 	then the one in inherited class
 *
 * when inheriting, the base constructor is called very first and the base destructor very last*/

namespace fileio
{

	class Userlist: public StorageReader
	{
	public:
		Userlist();
		~Userlist();	//doesn't need to be virtual, Userlist won't be further inherited

		void load_all_users();
		bool user_exists(std::string const & user) const;
	};

} /* namespace fileio */

#endif /* SRC_USERLIST_H_ */
