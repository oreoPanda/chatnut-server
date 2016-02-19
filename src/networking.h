/*
 * Client.h
 *
 *  Created on: Feb 7, 2016
 *      Author: i_fly_gliders
 */

#ifndef NETWORKING_H_
#define NETWORKING_H_

#include <iostream>	//for std::endl
#include <vector>

#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <errno.h>

#include "Buddy.h"

typedef int socket_t;

namespace networking
{

	class Buddy;

	class Horst //Beckmann
	{
	public:
		Horst(std::ostream * err, unsigned short port, int max);
		~Horst();
		bool init_success() const;
		socket_t accept_connection(struct sockaddr_in & addr) const;
		bool check_incoming() const;
	private:
		std::ostream * error_stream;
		socket_t sock;
		struct sockaddr_in address;
		int num_of_clients;
		int max_num_of_clients;
		bool open;
		bool usable;
		bool listening;
		void error(std::string const & msg, int errnum) const;
		/*void log(std::string const & msg) const;*/
	};

	class Client
	{
	public:
		Client(std::ostream * err, std::ostream * log, socket_t const sock, struct sockaddr_in const addr, Client * p);
		~Client();
		Client * Prev();
		Client * Next();
		void setPrev(Client * p);
		void setNext(Client * n);
		bool Connected() const;
		bool check_incoming() const;
		void Send(std::string & str);
		bool get_message(std::string & msg);
		std::forward_list<Buddy>::iterator add_buddy(std::string const & name);
	private:
		bool connected;
		std::ostream * error_stream;
		std::ostream * log_stream;
		socket_t sock;
		struct sockaddr_in address;
		bool login_status;
		std::string name;
		Client *prev;
		Client *next;
		std::forward_list<Buddy> buddies;
		bool receive(std::string & str, unsigned int len);
		void error(std::string const & msg) const;
		void error(std::string const & msg, int errnum) const;
		void log(std::string const & msg) const;
	};

}

/*namespace for functions used by both classes Horst and Client*/
namespace helpers
{

	bool check_incoming(socket_t sock, std::ostream * err, std::string const & className);

}

#endif /* NETWORKING_H_ */
