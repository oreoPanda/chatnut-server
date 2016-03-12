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

	}

	StorageReader::~StorageReader()
	{

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
		file.open( (receiver + "/" + from).c_str() );
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
					/*prepend the sender's name to the read message and store it*/
					std::string complete_message = from + " " + message;	//TODO check
					messages.push_back(complete_message);
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

	StorageWriter::StorageWriter(std::string const & rcv, std::string const & snd, std::string const & msg, fileio::LogWriter & logger)
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
