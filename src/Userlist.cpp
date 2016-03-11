/*
 * Userlist.cpp
 *
 *  Created on: Mar 11, 2016
 *      Author: i_fly_gliders
 */

#include "Userlist.h"

namespace fileio
{

	Userlist::Userlist()
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
					//TODO I/O error
				}
				else		//just failbit
				{
					//TODO logical error
				}
			}//end error check

		}//end file is open
		else
		{
			//TODO file couldn't be opened
			logger.error("Storage Reader", "Unable to open userlist at " + this->home + "/.chatnut-server/users", errno);
		}

		//TODO close file after loading userlist

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

} /* namespace fileio */
