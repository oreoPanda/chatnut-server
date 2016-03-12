/*
 * Userlist.h
 *
 *  Created on: Mar 11, 2016
 *      Author: i_fly_gliders
 */

#ifndef SRC_USERLIST_H_
#define SRC_USERLIST_H_

//C++ include directives
#include <fstream>
#include <vector>

//C include directives
#include <errno.h>

//my own include directives
#include "LogWriter.h"

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

	class Userlist
	{
	public:
		Userlist(fileio::LogWriter & logger);
		~Userlist();	//doesn't need to be virtual, Userlist won't be further inherited

		void load_all_users();
		bool user_exists(std::string const & user) const;
		bool check_password(std::string const & username, std::string const & password) const;
	private:
		std::vector<std::string> usernames;
		std::vector<std::string> passwords;
		std::ifstream file;		//TODO initialize in constructor?
		fileio::LogWriter & logger;
	};

} /* namespace fileio */

#endif /* SRC_USERLIST_H_ */
