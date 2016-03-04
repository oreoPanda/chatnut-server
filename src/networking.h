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

        class Client;
	class Buddy;
	class Action;

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
		std::forward_list<Buddy> reversebuddylist;
		bool receive(std::string & str, unsigned int len);
		void error(std::string const & msg) const;
		void error(std::string const & msg, int errnum) const;
		void log(std::string const & msg) const;
	};

	/*typedef enum
	{
		WHO
	}actiontype;*/

	/*mostly a storage class, stores an iterator to a buddy inside buddylist of a client that sent a /who command.
	 * actionreceiver is the name of the client that was added by the /who, the respective client will add the iterator
	 * from this action into his reverselist and set a pointer to himself and an iterator to the richt place in his reverselist
	 * in the element of the action's original creator buddylist*/
	/*Action currently only supports one iterator, it is wanted to expand supporting multiple iterators,
	 * each one set by a different /who request from a different client but to the same receiver. The receiver will
	 * only need to add all of the iterators in Action to his reverselist and edit all of the sender's buddylists as described above.
	 * The sender isn't stored because the iterator points directly at a spot in his buddylist.*/
	class Action
	{
	public:
                Action(std::string const & name, Client * const owner, std::forward_list<Buddy>::iterator const & i);
		~Action();
		std::string const & get_receiver() const;
                Client * get_owner();
		std::forward_list<Buddy>::iterator const & get_buddy_iter() const;
		//void add_object(std::string const & name, std::forward_list<networking::Buddy>::iterator const & iter);
		//bool pop_object_by_name(std::string const & name, networking::Buddy & b);
	private:
		/*actiontype type;*/
		std::string const actionreceiver;
		Client * const owner;
		std::forward_list<Buddy>::iterator const iter;
	};

}

/*namespace for functions used by both classes Horst and Client*/
namespace helpers
{

	bool check_incoming(socket_t sock, std::ostream * err, std::string const & className);

}

#endif /* NETWORKING_H_ */
