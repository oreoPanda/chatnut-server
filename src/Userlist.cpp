/*
 * Userlist.cpp
 *
 *  Created on: Mar 11, 2016
 *      Author: i_fly_gliders
 */

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
		std::string username;
		std::string password;

		/*open userlist*/
		file.open("users");
		if(file.is_open() )
		{
			/*read usernames and passwords from file and store them in the specified vectors*/
			/*TODO make more efficient by reading file as a whole and then separating*/
			while(file.good() )
			{
				//username
				getline(file, username);
				if(!file.good() )
				{
					break;
				}

				//password
				getline(file, password);
				/*check badbit and failbit, eofbit doesn't matter here*/
				if(file.fail() )
				{
					break;
				}

				//store them if both were read (break was not called)
				this->usernames.push_back(username);
				this->passwords.push_back(password);
			}//end reading file

			/*check whether file.failbit and/or file.badbit are set*/
			if(!file)
			{
				if(file.bad() )
				{
					this->logger.error("Userlist", "Unable to load userlist due to an I/O error", errno);
				}
				else		//just failbit
				{
					this->logger.error("Userlist", "Unable to load userlist due to a logical error");
				}
			}//end error check

		}//end file is open
		else
		{
			logger.error("Userlist", "Unable to open userlist", errno);
		}

		file.close();
		if(file.is_open() || file.fail() )
		{
			logger.error("Userlist", "Unable to close userlist", errno);
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
