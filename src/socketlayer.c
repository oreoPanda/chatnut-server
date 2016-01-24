/*socketlayer.c - for UNIX*/

#include "socketprx.h"

void reset_timeout(struct timeval *timeout)
{
	timeout->tv_sec = 0;
	timeout->tv_usec = 10,000; //microseconds: 1,000,000 of them each second, here process will sleep for one 100th of a second
}

/*exit with custom error message*/
void error_exit(char *error_message)
{
	fprintf( stderr, "%s: %s\n", error_message, strerror(errno) );

	exit(EXIT_FAILURE);
}

/*create a socket*/
int create_socket(int af, int type, int protocol)
{
	socket_t sock;
	const int y = 1;

	sock = socket( af, type, protocol );
	if( sock < 0 )		//check if it worked
	{
		error_exit("Error creating socket!");
	}
	
	setsockopt( sock, SOL_SOCKET, SO_REUSEADDR, &y, sizeof(int) );//SO_REUSEADDR makes the socket not be unusable for two minutes after server closes

	return sock;
}

void bind_socket(socket_t *sock, unsigned long address, unsigned short port)
{
	struct sockaddr_in server;

	memset( &server, 0, sizeof(server) );	//fill structure with zeroes to erase false data

	server.sin_family = AF_INET;
	server.sin_port = htons(port);
	server.sin_addr.s_addr = htonl(address);

	if( bind( *sock, (struct sockaddr*)&server, sizeof(server) ) < 0 )
	{
		error_exit("Error binding socket to port!" );
	}
}

void listen_socket(socket_t *sock)
{
	if( listen(*sock, CLIENT_MAX) == -1)		//will set up queue with length CLIENT_MAX, kernel has a longer queue so more than CLIENT_MAX unconnected clients might be connect()ed successfully
	{
		error_exit("Error listening for connections!");
	}
}

void connect_socket(socket_t *sock, char *server_addr, unsigned short port)
{
	struct sockaddr_in server;
	struct hostent *host_info;
	unsigned long ip_addr;

	memset( &server, 0, sizeof(server) );	//fill structure with zeroes to erase false data

	/*copy the ip address into the server address structure*/
	ip_addr = inet_addr(server_addr);	//turn ip into unsigned long
	if( ip_addr != INADDR_NONE )	//if it is a numeric ip
	{
		memcpy( (char*)&server.sin_addr, &ip_addr, sizeof(ip_addr) );//copy the unsigned long into the structure
	}
	else		//if it is a string address that needs to be resolved (like "localhost")
	{
		host_info = gethostbyname( server_addr );
		if( host_info == NULL )		//if resolving didn't quite work
		{
			error_exit("Error resolving server address (unknown server)!");
		}
		else
		{
			#define h_addr h_addr_list[0]
			memcpy( (char*)&server.sin_addr, host_info->h_addr, host_info->h_length );//copy the resolved address into structure
		}
	}

	/*copy the protocol family and the port number into the structure*/
	server.sin_family = AF_INET;
	server.sin_port = htons(port);

	/*connect to server and check if connecting worked*/
	if( connect( *sock, (struct sockaddr*)&server, sizeof(server) ) < 0)
	{
		error_exit("Error connecting to server!");
	}
	else
	{
		//say who the client is connected to
		printf( "Connected to server with address %s\n", inet_ntoa(server.sin_addr) );
	}
}

void my_select( int numfds, fd_set *readfds, fd_set *writefds, fd_set *exceptfds, struct timeval *timeout )
{
	if( select(numfds, readfds, writefds, exceptfds, timeout) == -1)//+1 was already done
	{
		error_exit("Error select()ing socket!");
	}
}


void accept_socket( socket_t *sock, socket_t *new_sock )
{
	struct sockaddr_in client;
	unsigned int len;

	len = sizeof(client);
	*new_sock = accept( *sock, (struct sockaddr*)&client, &len );
	if( *new_sock == -1 )	//if none found
	{
		error_exit("Error accepting connection!");
	}
	else
	{
		/*say who the server is connected to*/
		printf( "Connected to client with address %s\n", inet_ntoa(client.sin_addr) );

		/*char *msg = "hi\0";
		int len = 3;
		TCP_send( new_sock, msg, len, 0 );*/
	}
}

int TCP_send( socket_t *sock, const char *data, int len, int flags )
{
	
	int return_val;
	return_val = send( *sock, data, len, flags );
	if( return_val == -1 )
	{
		fprintf( stderr, "Error sending data!: %s\n", strerror(errno));
		return -1;
	}
	else if( return_val != len )
	{
		fprintf( stderr, "Not all data was sent!: %s\n", strerror(errno) );
	}
	else if( return_val == len )
	{
		printf( "All data was sent!\n" );
	}
	return len;

}

int TCP_recv( socket_t *sock, char *data, int len, int flags, int *status )
{
	fd_set readfds;
	struct timeval timeout;
	int highestfd;

	FD_ZERO( &readfds );
	FD_SET( *sock, &readfds );

	if( status != NULL )	//client gives a NULL to skip this part
	{	
		reset_timeout( &timeout );
		highestfd = *sock;
		printf( "inside TCP_recv, operation on sock %d, highestfd is %d\n", *sock, highestfd );
		my_select( highestfd+1, &readfds, NULL, NULL, &timeout );
	}

	if( FD_ISSET( *sock, &readfds ) )	//only try to receive if a read won't block
	{
		*status = SUCCESS;
		int return_val;
		return_val = recv( *sock, data, len, flags );
		if( return_val == -1 )
		{
			fprintf( stderr, "Error receiving data!: %s\n", strerror(errno) );
		}
		else if( return_val == 0 )
		{
			fprintf( stderr, "Looks like the other end is gone!: %s\n", strerror(errno) );
		}

		return return_val;
	}
	*status = FAILURE;		//set status to FAILURE if read will block (no message available)
	return FAILURE;		//I'm useless, will not be evaluated, only here to stop compiler warning "control reaches end of non-void function"
}

void cleanup(void)
{
	printf("Done cleaning up\n");
	return;
}

void close_socket( socket_t *sock )
{
	printf( "Trying to close socket %d\n", *sock );
	if( close(*sock) != 0 )
	{
		fprintf( stderr, "Error closing socket!: %s\n", strerror(errno) );
	}
}
