/*Storage.cpp*/

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
			//get direcory contents one by one
			while( (direntry = readdir(dir)) != NULL)
			{
				//leave out . and .. entries TODO leave out all other directories, too, just use files
				if(strcmp(direntry->d_name, ".") != 0 && strcmp(direntry->d_name, "..") != 0)
				{
					filelist.push_back(direntry->d_name);
				}
			}
			logger.log("Storage Reader", "There are some messages waiting to be sent to " + this->receiver);
		}
		else
		{
			logger.log("Storage Reader", "There are no messages waiting to be sent to " + this->receiver);
		}

		return filelist;
	}

	/*Remove files that contain messages for receiver
	 * TODO check if remove sets errno*/
	void StorageReader::remove_files(std::vector<std::string> const & files)
	{
		for(unsigned int i = 0; i < files.size(); i++)
		{
			if(remove( (this->receiver + '/' + files.at(i) ).c_str() ) != 0)
			{
				logger.error("Storage Reader", "Unable to remove file with messages from " + files.at(i) + " to " + this->receiver, errno);
			}
		}

		/*remove the directory*/
		if(rmdir(this->receiver.c_str()) != 0 )
		{
			logger.error("Storage Reader", "Unable to remove directory that contained messages for " + this->receiver, errno);
		}
	}

	/*reads all messages from a given user and stores them in the given messages vector
	 * returns true on success and false on failure
	 * warning: vector messages is cleared before usage and all previously stored content will be gone*/
	bool StorageReader::read_messages(std::string const & from, std::vector<std::string> & messages, char msg_indic)
	{
		file.open( (this->receiver + "/" + from).c_str() );
		if(file.is_open() )
		{
			/*clear the messages vector of any useless stuff*/
			if(messages.size() )
			{
				//TODO write a warn() function in LogWriter
				logger.log("Storage Reader", "Warning: Clearing non-empty vector in order to load messages.");
			}
			messages.clear();

			do
			{
				/*read one line from file and store it in the vector*/
				/*TODO increase efficiency by reading the whole file and then separating the messages. TODO do this
				 * in load_user_list() as well.*/
				std::string message;
				if(getline(file, message) )
				{
					/*prepend the sender's name to the read message and store it*/
					std::string complete_message = msg_indic + from + ' ' + message + '\n';
					messages.push_back(complete_message);
				}
			}
			while(file);

			file.clear();

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

		/*if the function reaches this point, directory was created or exists, so open the file and write the message to it*/
		file.open( (receiver + "/" + sender).c_str(), std::ios::app);
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

		return;
	}

} /* namespace fileio */
