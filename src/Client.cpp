/*
 * Client.cpp
 *
 *  Created on: Feb 7, 2016
 *      Author: i_fly_gliders
 */

#include "networking.h"

namespace networking
{

	/*Constructor, connect this to circle*/
	Client::Client(std::ostream * err, std::ostream * log, socket_t const sock, struct sockaddr_in const addr, Client * const p = NULL)
	:connected(true), error_stream(err), log_stream(log), sock(sock), address(addr), login_status(false), name(""), prev(p?p:this), next(p?p->Next():this)
	{
		/*edit the prev and next pointers of the adjacent objects in circle*/
		if(p)
		{
			p->setNext(this);
			next->setPrev(this);
		}
	}

	/*Destructor, close the socket and disconnect this from circle*/
	Client::~Client()
	{
		/*close the socket*/
		if( close(sock) == 0 )
		{
			connected = false;
		}
		else
		{
			error("Could not close socket: ", errno);
		}
		/*edit the prev and next pointers of the adjacent objects in circle*/
		prev->setNext(this->next);
		next->setPrev(this->prev);
	}

	/*return pointer to the previous element in circle*/
	Client * Client::Prev() const
	{
		return this->prev;
	}

	/*return pointer to next element in circle*/
	Client * Client::Next() const
	{
		return this->next;
	}

	/*set pointer to previous element in circle*/
	void Client::setPrev(Client * p)
	{
		this->prev = p;
	}

	/*set pointer to next element in circle*/
	void Client::setNext(Client * n)
	{
		this->next = n;
	}

	/*see whether or not the client is still connected*/
	bool Client::Connected() const
	{
		return this->connected;
	}

	/*handle actions by scanning through the list of actions, adding iterators to reverselist
	 * and setting up buddylist of affected clients*/
	void Client::handle_actions(std::forward_list<action::Action> & actions)
	{
		std::forward_list<action::Action>::iterator i;
		i = actions.begin();
		while(i != actions.end() )
		{
			if( (i->get_receiver().compare(this->name) ) == 0 )
			{
				reversebuddies.push_front(i->get_buddy_iter() );
			}
		}

		return;
	}

	/*check for an incoming message*/
	bool Client::check_incoming() const
	{
		return helpers::check_incoming(sock, error_stream, "Client");
	}

	/*receive a message*/
	bool Client::receive(std::string & str, unsigned int len)
	{
		int status;
		char *str_c = NULL;

		str_c = static_cast <char *> (calloc(len+1, sizeof(char) ) );

		status = recv(this->sock, str_c, len, 0);
		*(str_c+len) = 0;

		/*evaluate return value of recv()*/
		if( static_cast <unsigned int > (status) == len)
		{
			str = str_c;
			free(str_c);
			return true;
		}
		else if(status == 0 && len != 0)
		{
			log("Client has left, closing connection.");
			connected = false;
			free(str_c);
			return false;
		}
		/*TODO evaluate errno to see if client may have disconnected*/
		else if(status == -1)
		{
			error("Could not receive message: ", errno);
			//connected = false;
			free(str_c);
			return false;
		}
		else
		{
			error("Could not receive complete message.");
			str = str_c;
			free(str_c);
			return true;
		}
	}

	/*send a message or command reply to client (format for str: one byte indicator, then data, then '\n'*/
	/*TODO message is sent without the terminating NULL, that should not confuse clients (since client reads message line by line*/
	void Client::Send(std::string & str)
	{
		int status;

		status = send(this->sock, str.c_str(), str.length(), 0);

		/*evaluate return value of send()*/
		if( static_cast <unsigned int> (status) == str.length() )
		{
			return;
		}
		/*TODO evaluate errno to see if client may have disconnected*/
		else if(status == -1)
		{
			error("Could not send message: ", errno);
			connected = false;
			return;
		}
		/*TODO rather another try instead of setting connected to false*/
		else
		{
			error("Could not send complete message.");
			connected = false;
			return;
		}
	}

	/*receive a new incoming message as string*/
	bool Client::get_message(std::string & msg)
	{
		int len;
		std::string message;
		bool success;

		success = receive(message, 4);

		/*read the first 4 bytes, containing the length of the rest of the message*/
		if(success)
		{
			/*read the rest of the message*/ //(if message doesn't fill up len it's still used but an error message is printed)
			len = std::stoi(message, NULL);
			success = receive(message, len);
			if(success)
			{
				msg = message;
				return true;
			}
			else
			{
				return false;
			}
		}
		else
		{
			return false;
		}
	}

	std::string const & Client::get_name() const
	{
		return this->name;
	}

	void Client::error(std::string const & msg) const
	{
		*error_stream << "Client error: " << msg << std::endl;
	}

	/*write an error to the specified error stream*/
	void Client::error(std::string const & msg, int errnum) const
	{
		*error_stream << "Client error: " << msg << strerror(errnum) << std::endl;
	}

	void Client::log(std::string const & msg) const
	{
		*log_stream << "Client: " << msg << std::endl;
	}

	std::forward_list<Buddy>::iterator const Client::add_buddy(std::string const & name)
	{
		Buddy temp(name);
		buddies.push_front(temp);
		return buddies.begin();
	}

}	/* namespace networking */
