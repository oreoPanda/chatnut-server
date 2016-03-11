/*
 * Storage.h
 *
 *  Created on: Feb 7, 2016
 *      Author: i_fly_gliders
 */

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
		StorageReader(fileio::LogWriter & logger, std::string const & home);
		virtual ~StorageReader();	//has to be virtual, so that Userlist destructor is called too

		std::vector<std::string> get_file_list();
		bool read_messages(std::string const & from, std::vector<std::string> & messages);
		bool read();
	private:
		std::string receiver;
		std::ifstream file;
		fileio::LogWriter logger;
		std::vector<std::string> usernames;	//TODO to a class LoginReader or UserlistReader
		std::vector<std::string> passwords;	//TODO to a class LoginReader
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
		StorageWriter(std::string const & rcv, std::string const & snd, std::string const & msg, fileio::LogWriter const & logger);
		~StorageWriter();

		void write();
	private:
		std::string receiver;
		std::string sender;
		std::string message;
		std::ofstream file;
		fileio::LogWriter logger;
	};

} /* namespace fileio */

#endif /* SRC_STORAGE_H_ */
