/*Storage.h*/

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

#ifndef STORAGE_H_
#define STORAGE_H_

#include <fstream>
#include <vector>
#include <dirent.h>	//for working with directories
#include <errno.h>
#include <string.h>
#include <sys/stat.h>	//for mkdir	TODO check for a c++ way to chdir() and mkdir()
#include <unistd.h>

#include "LogWriter.h"

namespace fileio
{

	class Storage
	{
	public:
		Storage(std::string const & home, fileio::LogWriter & logger);
		virtual ~Storage();
		bool init_success() const;
	protected:
		bool dir_is_init;
		std::string const home;
		fileio::LogWriter & logger;

		void init_directory_structure();
	};

	class StorageReader
	{
	public:
		StorageReader(std::string const & rcv, fileio::LogWriter & logger);
		~StorageReader();

		std::vector<std::string> get_file_list();
		void remove_files(std::vector<std::string> const & files);
		bool read_messages(std::string const & from, std::vector<std::string> & messages, char msg_indic);
	private:
		std::string receiver;
		std::ifstream file;		//TODO initialize in constructor?
		fileio::LogWriter & logger;
		/*TODO check which constructors and destructors are called in inherited classes*/
	};

	/*This class stores messages that cannot be sent to receivers on the disk
	 * so they can be sent once the receivers come online
	 *
	 * In order to store a message you have to call:
	 * set_receiver()
	 * set_sender()
	 * set_message()
	 * write()
	 * It is important to call write last, and set the receiver before the sender.
	 * message can be set anytime*/
	class StorageWriter
	{
	public:
		StorageWriter(std::string const & rcv, std::string const & snd, std::string const & msg, fileio::LogWriter & logger);
		~StorageWriter();

		void write();
	private:
		std::string receiver;
		std::string sender;
		std::string message;
		std::ofstream file;
		fileio::LogWriter & logger;
	};

} /* namespace fileio */

#endif /* SRC_STORAGE_H_ */
