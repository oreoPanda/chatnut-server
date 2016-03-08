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
#include <string.h>
#include <sys/stat.h>	//for mkdir	TODO check for a c++ way to chdir() and mkdir()
#include <unistd.h>

#include "LogWriter.h"

namespace fileio
{

	class Storage
	{
	public:
		Storage(fileio::LogWriter & logger, std::string const & home);
		virtual ~Storage();
		void set_receiver(std::string const & rcv);
		void set_sender(std::string const & snd);
	protected:
		std::string message;
		std::string receiver;
		std::string sender;
		fileio::LogWriter & logger;
		std::string const home;
	};

	class StorageReader: public Storage
	{
	public:
		StorageReader(fileio::LogWriter & logger, std::string const & home);
		~StorageReader();
		bool init_success() const;
		void load_all_users();
		bool user_exists(std::string const & user) const;
		std::vector<std::string> get_file_list();
		std::string get_message() const;
		bool check_password(std::string const & username, std::string const & password) const;
		bool read();
	private:
		std::ifstream file;
		bool dir_is_init;
		std::vector<std::string> usernames;
		std::vector<std::string> passwords;

		void init_directory_structure();
	};

	class StorageWriter: public Storage
	{
	public:
		StorageWriter(fileio::LogWriter & logger, std::string const & home);
		~StorageWriter();
		void set_message(std::string const & msg);
		bool write();
	private:
		std::ofstream file;
	};

} /* namespace fileio */

#endif /* SRC_STORAGE_H_ */
