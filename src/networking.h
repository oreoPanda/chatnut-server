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

        class Action;
        class Buddy;

	class Horst //Beckmann     //TODO this could potentially be hazardous
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

		Client * Prev() const;
		Client * Next() const;
		void setPrev(Client * p);
		void setNext(Client * n);

		bool get_Connected() const;
		void set_Login(bool status);
		bool get_Login() const;
		void handle_actions(std::forward_list<Action> & actions);
		void set_buddy_data(std::forward_list<Buddy>::iterator const & buddy, Client * const cli);
		void unset_buddy_ptr(std::forward_list<Buddy>::iterator const & buddy);
		bool check_incoming() const;
		void Send(std::string const & str);
		bool get_message(std::string & msg);
		std::string const & get_name() const;
		std::forward_list<Buddy>::iterator const add_buddy(std::string const & name);
                std::forward_list<Buddy>::iterator const get_begin_buddy_iter();
                std::forward_list<Buddy>::iterator const get_end_buddy_iter();
                void advance_buddy_iter(std::forward_list<Buddy>::iterator & base) const;
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
		std::forward_list<Buddy> buddylist;
                std::forward_list<Buddy> reversebuddylist;     //rename to backtracebuddylist or something
		bool receive(std::string & str, unsigned int len);
		void error(std::string const & msg) const;
		void error(std::string const & msg, int errnum) const;
		void log(std::string const & msg) const;
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
                Action(Client * const creator, std::string const & recv_name, std::forward_list<Buddy>::iterator const & i);
		~Action();
                Client * get_creator();
		std::string const & get_receiver() const;
		std::forward_list<Buddy>::iterator const & get_buddy_iter() const;
	private:
		Client * const creator;
                std::string const receivername;
		std::forward_list<Buddy>::iterator const iter;
	};

}

/*namespace for functions used by both classes Horst and Client*/
namespace helpers
{

	bool check_incoming(socket_t sock, std::ostream * err, std::string const & className);

}

#endif /* NETWORKING_H_ */
