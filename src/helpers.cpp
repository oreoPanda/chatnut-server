/*
 * helpers.cpp
 *
 *  Created on: Feb 18, 2016
 *      Author: i_fly_gliders
 */

#include "networking.h"

namespace helpers
{

	/*check for a connection request or an incoming message*/
	bool check_incoming(socket_t sock, fileio::LogWriter & logger, std::string const & className)
	{
		fd_set readfds;
		struct timeval timeout;
		int highestfd;

		FD_ZERO (&readfds);
		FD_SET (sock, &readfds);

		timeout.tv_sec = 0;
		timeout.tv_usec = 10.000; //microseconds: 1,000,000 of them each second, here process will sleep for one 100th of a second
		highestfd = sock;

		//FIXME TODO timeouts with select() (and getch())	<-- not quite sure what I meant by that, it's long ago. Timeout length?
		if( select (highestfd+1, &readfds, NULL, NULL, &timeout) == -1)	//select() needs highest numbered fd + 1
		{
			logger.error(className, "Unable to check for incoming data", errno);
			return false;
		}
		else if( FD_ISSET (sock, &readfds) )	//if a connection is waiting
		{
			return true;
		}
		else
		{
			return false;
		}
	}

}	/* namespace helpers */
