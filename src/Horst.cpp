/*
 * Host.cpp
 *
 *  Created on: Feb 7, 2016
 *      Author: i_fly_gliders
 */

#include "networking.h"

using namespace helpers;

namespace networking
{

	/*Constructor, open socket and set it reusable*/
	Horst::Horst(unsigned short port, int max, fileio::LogWriter & logger)
	:sock(socket(AF_INET, SOCK_STREAM, 0)), num_of_clients(0), max_num_of_clients(max), open(false), usable(false), listening(false), logger(logger)
	{
		/*check if socket is open now*/
		if(sock != -1)
		{
			open = true;
		}
		else
		{
			logger.error("Horst", "Unable to create socket", errno);
		}

		/*set socket reusable*/
		int option_value = 1;
		if( setsockopt(sock, SOL_SOCKET, SO_REUSEADDR, &option_value, sizeof(option_value) ) != -1 )
		{
			usable = true;
		}
		else
		{
			logger.error("Horst", "Unable to set socket options", errno);
		}

		/*bind socket to the correct port*/
		memset(&address, 0, sizeof(address));
		address.sin_family = AF_INET;
		address.sin_port = htons(port);
		address.sin_addr.s_addr = htonl(INADDR_ANY);

		bind(sock, reinterpret_cast< struct sockaddr * >(&address), sizeof(struct sockaddr_in) );

		/*listen on the correct port*/
		if( listen(sock, max_num_of_clients) == 0 )
		{
			listening = true;
		}
		else
		{
			logger.error("Horst", "Unable to start listening on socket", errno);
		}
	}

	/*TODO write a copy constructor that calls error() with a warning that this shouldn't be copied*/

	/*Destructor, close socket*/
	Horst::~Horst()
	{
		if(open)
		{
			if( close(sock) == 0 )
			{
				open = false;
				usable = false;
				listening = false;
			}
			/*if socket can't be closed, the bools usable and listening are set to false for security*/
			else
			{
				open = true;
				usable = false;
				listening = false;
				logger.error("Horst", "Unable to close socket", errno);
			}
		}
		else
		{
			usable = false;
		}
	}

	/*Check if the creation, binding and listening on the socket works*/
	bool Horst::init_success() const
	{
		if( open && usable && listening )
		{
			return true;
		}
		else
		{
			return false;
		}
	}

	/*Accept an incoming connection*/
	socket_t Horst::accept_connection(struct sockaddr_in & addr) const
	{
		socket_t client;
		unsigned int length;
		struct sockaddr_in client_address;

		memset(&client_address, 0, sizeof(client_address));

		client = accept(sock, reinterpret_cast< struct sockaddr * >(&client_address), &length);
		if(client == -1)
		{
			memset(&addr, 0, sizeof(addr));
			logger.error("Horst", "Unable to accept connection", errno);
			return -1;
		}
		else
		{
			//TODO log a message
			return client;
		}
	}

	/*check for a connection request*/
	bool Horst::check_incoming() const
	{
		return helpers::check_incoming(sock, logger, "Horst");
	}

} /* namespace networking */
