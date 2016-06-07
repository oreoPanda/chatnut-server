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
				this->usernames.push_back(username);
				this->passwords.push_back(password);
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
			errorindicator = "";
			file.clear();	//set goodbit because failbit was set on opening failure
		}

		return;
	}

	/*check if a specified user exists by scanning through the usernames vector
	 * returns true if the specified user exists	TODO binary search
	 */
	bool Userlist::user_exists(std::string const & user) const
	{
		bool exists = false;

		for(unsigned int i = 0; i < usernames.size(); i++)
		{
			if(user.compare(usernames.at(i) ) == 0)
			{
				exists = true;
				break;
			}
		}

		return exists;
	}

	/*Look up the password to a given username by scanning TODO binary search.*/
	bool Userlist::check_password(std::string const & username, std::string const & password) const
	{
		bool password_true = false;

		if(!username.empty() )
		{
			for(unsigned int i = 0; i < this->usernames.size(); i++)
			{
				if(this->usernames.at(i).compare(username) == 0)
				{
					if(this->passwords.at(i).compare(password) == 0)
					{
						password_true = true;
					}
					break;
				}
			}
		}

		return password_true;
	}

} /* namespace fileio */
