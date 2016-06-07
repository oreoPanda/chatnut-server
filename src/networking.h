/*Client.h*/

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

#ifndef NETWORKING_H_
#define NETWORKING_H_

#include <forward_list>
#include <iostream>	//for std::endl
#include <list>
#include <vector>

#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <errno.h>

#include "Buddy.h"
#include "LogWriter.h"

typedef int socket_t;

namespace networking
{

        class Action;
        class Buddy;

	class Horst
	{
	public:
		Horst(unsigned short port, int max, fileio::LogWriter & logger);
		~Horst();
		bool init_success() const;
		socket_t accept_connection(struct sockaddr_in & addr) const;
		bool check_incoming() const;
	private:
		socket_t sock;
		struct sockaddr_in address;
		int num_of_clients;
		int max_num_of_clients;
		bool open;
		bool usable;
		bool listening;
		fileio::LogWriter & logger;
	};

	class Client
	{
	public:
		Client(socket_t const sock, struct sockaddr_in const addr, Client * const p = NULL, fileio::LogWriter * const logger = NULL);
		~Client();

		Client * Prev() const;
		Client * Next() const;
		void setPrev(Client * p);
		void setNext(Client * n);

		//TODO check order of functions
		bool get_Connected() const;
		void set_Login(bool status);
		bool get_Login() const;
		void handle_actions(std::forward_list<Action> & actions);
		void set_buddy_data(std::list<Buddy>::iterator const & buddy, Client * const cli, std::list<Buddy>::iterator const & iter);
		void unset_buddy_ptr(std::list<Buddy>::iterator const & buddy);
		void clear_buddylist();
		void remove_from_reversebuddylist(std::list<Buddy>::iterator const & reverselistiter);
		bool check_incoming() const;
		void Send(std::string const & str);
		bool get_message(std::string & msg);
		void set_Name(std::string const & n);
		std::string get_Name() const;
		std::list<Buddy>::iterator const add_buddy(std::string const & name);
		std::list<Buddy>::iterator const get_begin_buddy_iter();
		std::list<Buddy>::iterator const get_end_buddy_iter();
		void advance_buddy_iter(std::list<Buddy>::iterator & base) const;
	private:
		bool connected;
		socket_t sock;
		struct sockaddr_in address;
		bool login_status;
		std::string name;
		Client *prev;
		Client *next;
		std::list<Buddy> buddylist;		//TODO turn into a forward_list
		std::list<Buddy> reversebuddylist;     //TODO rename to backtracebuddylist or something
		fileio::LogWriter & logger;
		bool receive(std::string & str, unsigned int len);
	};

	/*a storage and information transfer class used for the /who command
         * owner:           pointer to the commanding client
	 * receivername:    the name of the client requested by /who (the new buddy of the commanding client). It will receive this action.
         * iter:            an iterator of the creator's buddylist pointing at the place where the receiver's name and later a pointer to him are stored
         */
	/*At the moment, each /who creates one action.
         * In the future, there will only be one action per receiver
         *      owner and iter will have to be some array, vector or (forward) list
         *          each /who request will add one owner and an iterator of his buddylist to the right action
         */
	class Action
	{
	public:
                Action(Client * const creator, std::string const & recv_name, std::list<Buddy>::iterator const & i);
		~Action();
                Client * get_creator();
		std::string const & get_receiver() const;
		std::list<Buddy>::iterator const & get_buddy_iter() const;
	private:
		Client * const creator;
                std::string const receivername;
		std::list<Buddy>::iterator const iter;
	};

}

/*namespace for functions used by both classes Horst and Client*/
namespace helpers
{

	bool check_incoming(socket_t sock, fileio::LogWriter & logger, std::string const & className);

}

#endif /* NETWORKING_H_ */
