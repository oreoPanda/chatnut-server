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
	Client::Client(socket_t const sock, struct sockaddr_in const addr, Client * const p, fileio::LogWriter * const logger)
	:connected(true), sock(sock), address(addr), login_status(false), name(""), prev(p?p:this), next(p?p->Next():this), logger(*logger)
	{
		/*edit the prev and next pointers of the adjacent objects in circle*/
		if(p)
		{
			p->setNext(this);
			next->setPrev(this);
		}
	}

	/*Destructor, close the socket, tell everyone stored in reversebuddylist that this is gone and disconnect this from circle*/
	Client::~Client()
	{
		/*close the socket*/
		if( close(sock) == 0 )
		{
			connected = false;
		}
		else
		{
			logger.error("Client", "Unable to close socket", errno);
		}

		/*inform reverse clients that this is gone*/
		std::list<Buddy>::iterator reverseiter;
		reverseiter = reversebuddylist.begin();
		while(reverseiter != reversebuddylist.end() )
		{
			reverseiter->get_client()->unset_buddy_ptr(reverseiter->get_iter() );
			reverseiter++;
			reversebuddylist.pop_front();
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
	bool Client::get_Connected() const
	{
		return this->connected;
	}

	void Client::set_Login(bool status)
	{
		this->login_status = status;
		return;
	}

	bool Client::get_Login() const
	{
		return this->login_status;
	}

	/*handle actions by scanning through the list of actions, adding iterators to reverselist
	 * and setting up buddylist of affected clients*/
	void Client::handle_actions(std::forward_list<Action> & actions)
	{
		std::forward_list<Action>::iterator i;
		std::forward_list<Action>::iterator last;
		i = actions.begin();
		while(i != actions.end() )
		{
			if( (i->get_receiver().compare(this->name) ) == 0 )
			{
				/*create a new buddy object and add it to our reverselist*/
				Buddy temp;
				temp.set_client(i->get_creator() );
				temp.set_iter(i->get_buddy_iter() );
				reversebuddylist.push_front(temp);

				/*tell the owner of the action where he stored us,
				 * who we are and where to find himself in our reverselist (for when he leaves or sends /unwho)*/
				i->get_creator()->set_buddy_data(i->get_buddy_iter(), this, this->reversebuddylist.begin() );

				/*remove the action from list*/
				if(i == actions.begin() )
				{
					//remove from beginning
					i++;
					actions.pop_front();
				}
				else
				{
					i = actions.erase_after(last);
				}
			}
			else
			{
				last = i;
				i++;
			}
		}

		return;
	}

	/*in buddylist at position buddy, set client and iterator of buddy object*/
	void Client::set_buddy_data(std::list<Buddy>::iterator const & buddy, Client * const cli, std::list<Buddy>::iterator const & iter)
	{
		buddy->set_client(cli);
		buddy->set_iter(iter);
		return;
	}

	/*remove the client pointer from buddylist at position that buddy points at
	 * won't remove the iter, but iter shall not be used anyways because it doesn't point at anything valuable anymore*/
	void Client::unset_buddy_ptr(std::list<Buddy>::iterator const & buddy)
	{
		buddy->set_client(NULL);
		return;
	}

	/*communicate to all buddies that they can remove this from his reversebuddylist,
	 * then clear buddylist*/
	void Client::clear_buddylist()
	{
		std::list<Buddy>::iterator buddylistiter = buddylist.begin();
		/*communicate to others that this no longer has buddies*/
		while(buddylistiter != buddylist.end() )
		{
			buddylistiter->get_client()->remove_from_reversebuddylist(buddylistiter->get_iter() );
			buddylistiter++;
		}

		/*clear buddylist*/
		buddylist.clear();
	}

	/*Removes the element that reverselistiter points at from reversebuddylist*/
	void Client::remove_from_reversebuddylist(std::list<Buddy>::iterator const & reverselistiter)
	{
		reversebuddylist.erase(reverselistiter);
	}

	/*check for an incoming message*/
	bool Client::check_incoming() const
	{
		return helpers::check_incoming(sock, this->logger, "Client");
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
			logger.log("Client", "Client has left, closing connection");
			connected = false;
			free(str_c);
			return false;
		}
		/*TODO evaluate errno to see if client may have disconnected*/
		else if(status == -1)
		{
			logger.error("Client", "Unable to receive message", errno);
			//connected = false;
			free(str_c);
			return false;
		}
		else
		{
			logger.error("Client", "Unable to receive complete message");
			str = str_c;
			free(str_c);
			return true;
		}
	}

	/*send a message or command reply to client (format for str: one byte indicator, then data, then '\n'*/
	/*TODO message is sent without the terminating NULL, that should not confuse clients (since client reads message line by line*/
	void Client::Send(std::string const & str)
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
			logger.error("Client", "Unable to send message", errno);
			connected = false;
			return;
		}
		/*TODO rather another try instead of setting connected to false*/
		else
		{
			logger.error("Client", "Unable to send complete message");
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

	std::list<Buddy>::iterator const Client::add_buddy(std::string const & name)
	{
		Buddy temp;
		temp.set_name(name);
		buddylist.push_front(temp);
		return buddylist.begin();
	}
	
	std::list<Buddy>::iterator const Client::get_begin_buddy_iter()
	{
		return buddylist.begin();
	}
        
	std::list<Buddy>::iterator const Client::get_end_buddy_iter()
	{
		return buddylist.end();
	}
	
	void Client::advance_buddy_iter(std::list<Buddy>::iterator & base) const
	{
		base++;

		return;
	}

}	/* namespace networking */
