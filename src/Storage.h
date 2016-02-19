/*
 * Storage.h
 *
 *  Created on: Feb 7, 2016
 *      Author: i_fly_gliders
 */

#ifndef STORAGE_H_
#define STORAGE_H_

#include <fstream>

namespace fileio
{

	class Storage
	{
	public:
		Storage();
		virtual ~Storage();
		void set_receiver(std::string const & rcv);
		void set_sender(std::string const & snd);
	protected:
		std::string message;
		std::string receiver;
		std::string sender;
	};

	class StorageReader: public Storage
	{
	public:
		StorageReader();
		~StorageReader();
		std::string & get_message();
		bool read();
	private:
		std::ifstream file;
	};

	class StorageWriter: public Storage
	{
	public:
		StorageWriter();
		~StorageWriter();
		void set_message(std::string const & msg);
		bool write();
	private:
		std::ofstream file;
	};

} /* namespace fileio */

#endif /* SRC_STORAGE_H_ */
