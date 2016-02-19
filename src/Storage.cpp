/*
 * Storage.cpp
 *
 *  Created on: Feb 7, 2016
 *      Author: i_fly_gliders
 */

#include <iostream>
#include <unistd.h>

#include "Storage.h"

namespace fileio
{

	Storage::Storage()
	{
		std::cout << "Storage Constructor." << std::endl;
	}

	Storage::~Storage()
	{
		std::cout << "Storage Destructor." << std::endl;
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

	StorageReader::StorageReader()
	{
		std::cout << "StorageReader Constructor." << std::endl;
	}

	StorageReader::~StorageReader()
	{
		std::cout << "StorageReader Destructor." << std::endl;

		if(file.is_open())
		{
			file.close();
		}
	}

	/*TODO maybe use const casts in return and make this function const*/
	std::string & StorageReader::get_message()
	{
		return message;
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

	StorageWriter::StorageWriter()
	{
		std::cout << "StorageWriter Constructor." << std::endl;
	}

	StorageWriter::~StorageWriter()
	{
		std::cout << "StorageWriter Destructor." << std::endl;

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
