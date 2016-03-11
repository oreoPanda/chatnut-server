/*
 * Storage.cpp
 *
 *  Created on: Feb 7, 2016
 *      Author: i_fly_gliders
 */

#include "Storage.h"

namespace fileio
{

	Storage::Storage(std::string const & home, fileio::LogWriter & logger)
	:dir_is_init(false), home(home), logger(logger)
	{
		/*initialize directory structure*/
		init_directory_structure();
	}
	
	Storage::~Storage()
	{
		
	}

	/* - switches into the user's home directory
	 * - creates the directory .chatnut-server there
	 * - sets dir_is_init to true on success, call init_success to check*/
	void Storage::init_directory_structure()
	{
		if( chdir(this->home.c_str()) != 0 )
		{
			logger.error("Storage", "Unable to switch to " + this->home, errno);
			this->dir_is_init = false;
			return;
		}
		if( mkdir( ".chatnut-server", 0755 ) != 0 )
		{
			if( errno != EEXIST )
			{
				logger.error("Storage", "Unable to create directory " + this->home + "/.chatnut-server", errno );
				this->dir_is_init = false;
				return;
			}
		}
		if( chdir(".chatnut-server") != 0 )
		{
			logger.error("Storage", "Unable to switch to " + this->home + "/.chatnut-server", errno);
			this->dir_is_init = false;
			return;
		}

		this->dir_is_init = true;
		return;
	}

	/*check if the initialization was successful*/
	bool Storage::init_success() const
	{
		return this->dir_is_init;
	}

	StorageReader::StorageReader(std::string const & rcv, fileio::LogWriter & logger)
	:receiver(rcv), logger(logger)
	{
		/*load userlist*/
		load_all_users();	//TODO only in UserlistReader
	}

	StorageReader::~StorageReader()
	{
		if(file.is_open())
		{
			file.close();
		}
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
			logger.error("Storage Reader", "Unable to open userlist", errno);
		}
		
		//TODO close file after loading userlist

		return;
	}
	
	/*Look up the password to a given username by scanning TODO binary search.*/
	bool StorageReader::check_password(std::string const & username, std::string const & password) const
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

	/*check if a specified user exists by scanning through the usernames vector
	 * returns true if the specified user exists	TODO binary search
	 */
	bool StorageReader::user_exists(std::string const & user) const
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

	/*returns a vector of the filenames inside the receiver's directory*/
	std::vector<std::string> StorageReader::get_file_list()
	{
		std::vector<std::string> filelist;

		//create a list of all files
		DIR * dir = NULL;
		struct dirent * direntry = NULL;

		if( (dir = opendir(receiver.c_str() )) != NULL)
		{
			while( (direntry = readdir(dir)) != NULL)
			{
				filelist.push_back(direntry->d_name);
			}
		}
		else
		{
			logger.error("Storage Writer", "Unable to open subdirectory " + this->receiver, errno);
		}

		return filelist;
	}

	/*reads all messages from a given user and stores them in the given messages vector
	 * returns true on success and false on failure
	 * warning: vector messages is cleared before usage and all previously stored content will be gone*/
	bool StorageReader::read_messages(std::string const & from, std::vector<std::string> & messages)
	{
		file.open( receiver.c_str() + "/" + from.c_str() );
		if(file.is_open() )
		{
			/*clear the messages vector of any useless stuff*/
			if(messages.size() )
			{
				//TODO write a warn() function in LogWriter
				logger.log("Storage Reader", "Warning: Clearing non-empty vector in order to load messages.");
			}
			messages.clear();

			while(file.good() )
			{
				/*read one line from file and store it in the vector*/
				/*TODO increase efficiency by reading the whole file and then separating the messages. TODO do this
				 * in load_user_list() as well.*/
				std::string message;
				getline(file, message);	//TODO check when eof happens: if it happens in the last line, the following if-statement
												//is useless. if it happens after last line, the if-statement is okay
				if(!message.empty() )
				{
					messages.push_back(message);
				}
			}
			file.close();
			if(file.is_open() || file.fail() )	//TODO check which ones to check
			{
				logger.error("Storage Reader", "Unable to close file " + this->receiver + "/" + from, errno);
			}

			return true;
		}
		else
		{
			logger.error("Storage Reader", "Unable to open file " + this->receiver + "/" + from, errno);
			return false;
		}
	}

	StorageWriter::StorageWriter(std::string const & rcv, std::string const & snd, std::string const & msg, fileio::LogWriter const & logger)
	:receiver(rcv), sender(snd), message(msg), logger(logger)
	{}

	StorageWriter::~StorageWriter()
	{
		if(file.is_open())
		{
			file.close();	//TODO check if it worked, error message if not, check if it is needed
		}
	}

	/*write a message into temporary storage.
	 * Returns true if message was written and false if not.
	 * TODO state flag or exception if switching back to ../ fails*/
	void StorageWriter::write()
	{
		/*create receiver's directory if it doesn't exist yet*/
		if( mkdir( this->receiver.c_str(), 0755 ) != 0 )
		{
			if( errno != EEXIST )
			{
				logger.error("Storage Writer", "Unable to create subdirectory " + this->receiver, errno );
				return;
			}
		}
		else	//directory was created or already exists
		{
			/*open the file and write the message to it*/
			file.open( (receiver + sender).c_str() );
			if(file.is_open() )
			{
				file << message << std::endl;
				if(file.bad() )
				{
					logger.error("Storage Writer", "Unable to write message to " + this->receiver + "/" + this->sender);
				}

				/*close the file again*/
				file.close();
				if(file.is_open() || file.fail() )	//TODO check which one to use
				{
					logger.error("Storage Writer", "Unable to close file " + this->receiver + "/" + this->sender, errno);
				}
			}//end file is open
			else
			{
				logger.error("Storage Writer", "Unable to open file " + this->receiver + "/" + this->sender, errno);
			}
		}//end else (directory was created)

		return;
	}

} /* namespace fileio */
