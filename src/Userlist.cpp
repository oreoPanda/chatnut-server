/*Userlist.cpp*/

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

#include "Userlist.h"

namespace fileio
{

	Userlist::Userlist(fileio::LogWriter & logger)
	:logger(logger)
	{
		/*load userlist*/
		load_all_users();
	}

	Userlist::~Userlist()
	{

	}

	void Userlist::load_all_users()
	{
		std::string errorindicator = "out";		//to write without :D
		std::string username;
		std::string password;

		this->logger.log("Userlist", "Loading userlist..");

		/*open userlist*/
		file.open("users");
		if(file.is_open() )
		{
			/*read usernames and passwords from file and store them in the specified vectors*/
			/*TODO make more efficient by reading file as a whole and then separating*/
			while(file.good() )
			{
				//username
				std::getline(file, username);
				if(file.eof() )
				{
					break;
				}

				//password
				std::getline(file, password);
				if(file.eof() )
				{
					this->logger.error("Userlist", "No password supplied for last user in list");
					errorindicator = "";
					break;
				}

				//store them if both were read (break was not called)
				std::string user[2] = {username, password};
				this->users.push_back(user);
			}//end reading file

			/*check for errors*/
			if(file.bad() )
			{
				this->logger.error("Userlist", "Unable to load userlist due to an I/O error", errno);
				errorindicator = "";
			}
			else if(file.fail() )
			{
				if(!file.eof() )
				{
					this->logger.error("Userlist", "Unable to load userlist due to a logical error");
					errorindicator = "";
				}
			}

			/*clear errors before closing*/
			file.clear();

			/*close the file*/
			file.close();
			/*check for new errors*/
			if(file.is_open() || file.fail() )
			{
				logger.error("Userlist", "Unable to close userlist", errno);
				errorindicator = "";
			}

			/*final log message*/
			this->logger.log("Userlist", "Loaded userlist with" + errorindicator + " errors");

		}//end file is open
		else	//file is not open
		{
			logger.error("Userlist", "Unable to open userlist", errno);
			file.clear();	//set goodbit because failbit was set on opening failure
		}

		return;
	}

	/*uses binary search to look up a user from the userlist
	 * if user exists, its position is returned and i is set to that position.
	 * if user doesn't exist, the position in front of which the user could be added is the negative return value. i is set to that position.*/
	int Userlist::find_user(std::string const & user, std::list<std::string[2]>::iterator & iter)
	{
		int center;
		int iterpos;
		int min = 0;
		int max = users.size()-1;

		iter = users.begin();
		iterpos = 0;

		//start searching
		while(min <= max)
		{
			//calculate center and move iter there
			center = min + (max-min)/2;
			if(iterpos < center)
			{
				for(int i = 0; i < center-iterpos; i++)
				{
					iter++;
				}
			}
			else if(iterpos > center)
			{
				for(int i = 0; i < iterpos-center; i++)
				{
					iter--;
				}
			}
			iterpos = center;

			//check if user must be left or right of where i is at
			int test = (*iter)[0].compare(user);	//compare the user at i to user
			if(test < 0)
			{
				min = center+1;			//continue search in upper half
			}
			else if(test > 0)
			{
				max = center-1;			//continue search in lower half
			}
			else
			{
				return center;
			}
		}

		//min is the position in front of which the user could be added, set iter to that position
		if(iterpos > min)
		{
			for(int i = 0; i < iterpos-min; i++)
			{
				iter--;
			}
		}
		else if(iterpos < min)
		{
			for(int i = 0; i < min-iterpos; i++)
			{
				iter++;
			}
		}
		iterpos = min;

		return -iterpos;		//the user doesn't exist but can be added in front of iter
	}

	/*check if a user exists
	 * returns true if user exists
	 * returns false if user doesn't exist*/
	bool Userlist::user_exists(std::string const & username)
	{
		std::list<std::string[2]>::iterator i;
		int n = find_user(username, i);
		if(n >= 0)
		{
			return true;
		}
		else return false;
	}

	/*check the password for a given user TODO hashing*/
	bool Userlist::check_password(std::string const & username, std::string const & password)
	{
		if(!username.empty() )
		{
			std::list<std::string[2]>::iterator i;
			int pos = find_user(username, i);
			if( pos < 0 )
			{
				return false;		//user doesn't exist
			}
			else if( (*i)[1].compare(password) == 0)
			{
				return true;		//password correct
			}
			else
			{
				return false;		//password incorrect
			}
		}

		return false;
	}

	/*adds a new user to userlist in the correct position and rewrites userlist to file.
	 * returns true on success
	 * returns false on failure or if user already exists*/
	bool Userlist::register_new_user(std::string const & username, std::string const & password)
	{
		std::list<std::string[2]>::iterator i;
		int pos = -find_user(username, i);
		if(pos >= 0)
		{
			std::string userdata[2] = {username, password};
			users.insert(i, userdata);

			/*write all users to file*/
			std::string errorindicator = "out";		//to write without :D

			this->logger.log("Userlist", "Rewriting userlist..");

			/*open userlist*/
			ofile.open("users");
			if(ofile.is_open() )
			{
				/*write all users into one string called data*/
				std::string data;
				i = users.begin();
				while(i != users.end())
				{
					data += (*i)[0];
					data += "\n";
					data += (*i)[1];
					data += "\n";
					i++;
				}

				/*write data to file*/
				ofile << data;

				/*check for errors*/
				if(ofile.bad() )
				{
					this->logger.error("Userlist", "Unable to rewrite userlist due to an I/O error", errno);
					errorindicator = "";
				}
				else if(ofile.fail() )
				{
					this->logger.error("Userlist", "Unable to rewrite userlist due to a logical error");
					errorindicator = "";
				}

				/*clear errors before closing*/
				ofile.clear();

				/*close the file*/
				ofile.close();
				/*check for new errors*/
				if(ofile.is_open() || ofile.fail() )
				{
					logger.error("Userlist", "Unable to close userlist", errno);
					errorindicator = "";
				}

				/*final log message*/
				this->logger.log("Userlist", "Rewrote userlist with" + errorindicator + " errors");

				return true;		//registered new user

			}//end file is open
			else	//file is not open
			{
				logger.error("Userlist", "Unable to open userlist", errno);
				ofile.clear();	//set goodbit because failbit was set on opening failure

				return false;		//didn't register new user
			}
		}
		else
		{
			return false;		//user already exists
		}
	}

} /* namespace fileio */
