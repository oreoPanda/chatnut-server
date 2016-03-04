/*
 * Storage.h
 *
 *  Created on: Feb 7, 2016
 *      Author: i_fly_gliders
 */

#ifndef STORAGE_H_
#define STORAGE_H_

#include <string.h>
#include <sys/stat.h>	//for mkdir	TODO check for a c++ way to chdir() and mkdir()

#include <fstream>
#include <vector>

namespace fileio
{

	class Storage
	{
	public:
		Storage(std::ostream * const err);
		virtual ~Storage();
		void set_receiver(std::string const & rcv);
		void set_sender(std::string const & snd);
		void error(std::string const & msg, int errnum) const;
	protected:
		std::string message;
		std::string receiver;
		std::string sender;
		std::ostream * const error_stream;
	};

	class StorageReader: public Storage
	{
	public:
		StorageReader(std::ostream * const err);
		~StorageReader();
		void init_directory_structure();
		bool init_success() const;
		void load_all_users();
		std::string get_message() const;
		bool check_password_for(std::string const & username, std::string const & password) const;
		bool read();
	private:
		std::ifstream file;
		bool dir_is_init;
		std::vector<std::string> usernames;
		std::vector<std::string> passwords;
	};

	class StorageWriter: public Storage
	{
	public:
		StorageWriter(std::ostream * const err);
		~StorageWriter();
		void set_message(std::string const & msg);
		bool write();
	private:
		std::ofstream file;
	};

} /* namespace fileio */

#endif /* SRC_STORAGE_H_ */
