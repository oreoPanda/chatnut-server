/*Userlist.h*/

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
