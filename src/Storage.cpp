/*
 * Storage.cpp
 *
 *  Created on: Feb 7, 2016
 *      Author: i_fly_gliders
 */

#include <iostream>
#include <unistd.h>
#include <errno.h>

#include "Storage.h"

namespace fileio
{

	Storage::Storage(std::ostream * const err)
	:error_stream(err)
	{
		
	}

	/*TODO check which constructors and destructors are called in inherited classes*/
	
	Storage::~Storage()
	{
		
	}

	void Storage::set_receiver(std::string const & rcv)
	{
		receiver = rcv;
		return;
	}

	void Storage::set_sender(std::string const & snd)
	{
		sender = snd;
		return;
	}

	/*write an error to the specified error stream*/
	void Storage::error(std::string const & msg, int errnum) const
	{
		*error_stream << "Storage error: " << msg << strerror(errnum) << std::endl;
	}

	StorageReader::StorageReader(std::ostream * const err)
	:Storage(err), dir_is_init(false)
	{
		/*initialize directory structure*/
		init_directory_structure();

		/*load userlist*/
		load_all_users();
	}

	StorageReader::~StorageReader()
	{
		

		if(file.is_open())
		{
			file.close();
		}
	}

	void StorageReader::init_directory_structure()
	{
		if( chdir(getenv("HOME")) != 0 )
		{
			fprintf( stderr, "Error switching into your home directory: %s\n", strerror(errno) );
			this->dir_is_init = false;
			return;
		}
		if( mkdir( ".chatnut-server", 0755 ) != 0 )
		{
			if( errno != EEXIST )
			{
				//TODO error in parent class Storage
				error("Error creating directory .chatnut-server in your home directory", errno );
				this->dir_is_init = false;
				return;
			}
		}
		if( chdir(".chatnut-server") != 0 )
		{
			fprintf( stderr, "Error switching to .chatnut-server in your home directory: %s\n", strerror(errno) );
			this->dir_is_init = false;
			return;
		}

		this->dir_is_init = true;
		return;
	}

	/*check if the initialization was successful*/
	bool StorageReader::init_success() const
	{
		return this->dir_is_init;
	}

	void StorageReader::load_all_users()
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
			Storage::error("The userlist couldn't be opened: ", errno);
		}
		
		//TODO close file after loading userlist

		return;
	}
	
	/*returns a vector of the filenames inside the receiver's directory*/
	std::vector<std::string> StorageReader::get_file_list()
	{
		std::vector<std::string> filelist;

		if( chdir(receiver.c_str() ) == 0 )
		{
			//create a list of all files
			//http://openbook.rheinwerk-verlag.de/unix_guru/node383.html
		}
		else
		{
			//TODO error unable to switch into receiver's directory
		}

		return filelist;
	}

	/*Look up the password to a given username by scanning TODO binary search.*/
	bool StorageReader::check_password_for(std::string const & username, std::string const & password) const
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

	bool StorageReader::read()
	{
		bool success = false;

		if( chdir(receiver.c_str()) == 0 )
		{
			file.open( sender.c_str() );
			if(file.is_open())
			{
				file >> message;
				if(file.eof())
				{
					success = false;
					file.clear();
				}
				else
				{
					success = true;
				}
				file.close();
			}
			else
			{
				success = false;
			}

			/*switch back to parent directory*/
			if( chdir("../") == 0 )
			{
				//throw an exception or something (state flag of this class??) cuz unable to switch back to ../
			}

			return success;
		}
		else
		{
			return false;
		}
	}

	StorageWriter::StorageWriter(std::ostream * const err)
	:Storage(err)
	{
		
	}

	StorageWriter::~StorageWriter()
	{
		

		if(file.is_open())
		{
			file.close();
		}
	}

	void StorageWriter::set_message(std::string const & msg)
	{
		message = msg;
		return;
	}

	/*write a message into temporary storage.
	 * Returns true if message was written and false if not.
	 * TODO state flag or exception if switching back to ../ fails*/
	bool StorageWriter::write()
	{
		bool success = false;

		if( chdir(receiver.c_str()) == 0 )
		{
			file.open( sender.c_str() );
			if(file.is_open())
			{
				file << message << std::endl;
				if(file.bad())
				{
					success = false;
					file.clear();
				}
				else
				{
					success = true;
				}
				file.close();
			}
			else
			{
				success = false;
			}

			/*switch back to parent directory*/
			if( chdir("../") == 0 )
			{
				//throw an exception or something (state flag of this class??) cuz unable to switch back to ../
			}

			return success;
		}
		else
		{
			return false;
		}
	}

} /* namespace fileio */
