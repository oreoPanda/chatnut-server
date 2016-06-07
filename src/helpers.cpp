/*helpers.cpp*/

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
