/*socketlayer.c - for UNIX*/

#include "socketprx.h"
#include <math.h>

void reset_timeout(struct timeval *timeout)
{
	timeout->tv_sec = 0;
	timeout->tv_usec = 10,000; //microseconds: 1,000,000 of them each second, here process will sleep for one 100th of a second
}

/*print an error*/
void print_socket_error( char *error_message, boolean newline )
{
	fprintf( stderr, "%s", error_message );
	if(newline)
	{
		fprintf( stderr, "\n" );
	}

	return;
}

/*create a socket*/
int create_socket(int af, int type, int protocol)
{
	socket_t sock;
	const int y = 1;

	sock = socket( af, type, protocol );
	if( sock < 0 )		//check if it worked
	{
		print_socket_error( "Error creating socket: ", FALSE);
		print_socket_error( strerror(errno), TRUE );
	}
	
	setsockopt( sock, SOL_SOCKET, SO_REUSEADDR, &y, sizeof(int) );//SO_REUSEADDR makes the socket not be unusable for two minutes after server closes

	return sock;
}

status bind_socket(socket_t *sock, unsigned long address, unsigned short port)
{
	struct sockaddr_in server;

	memset( &server, 0, sizeof(server) );	//fill structure with zeroes to erase false data

	server.sin_family = AF_INET;
	server.sin_port = htons(port);
	server.sin_addr.s_addr = htonl(address);

	if( bind( *sock, (struct sockaddr*)&server, sizeof(server) ) < 0 )
	{
		print_socket_error( "Error binding socket to port: ", FALSE );
		print_socket_error( strerror(errno), TRUE );
		return FAILURE;
	}

	return SUCCESS;
}

status listen_socket(socket_t *sock)
{
	if( listen(*sock, CLIENT_MAX) == -1)		//will set up queue with length CLIENT_MAX, kernel has a longer queue so more than CLIENT_MAX unconnected clients might be connect()ed successfully
	{
		print_socket_error( "Error listening for connections: ", FALSE );
		print_socket_error( strerror(errno), TRUE );
		return FAILURE;
	}
	else
	{
		return SUCCESS;
	}
}

void my_select( int numfds, fd_set *readfds, fd_set *writefds, fd_set *exceptfds, struct timeval *timeout )
{
	if( select(numfds, readfds, writefds, exceptfds, timeout) == -1)//+1 was already done
	{
		print_socket_error( "Error select()ing socket: ", FALSE );
		print_socket_error( strerror(errno), TRUE );
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
		print_socket_error( "(accept_socket)Error accepting connection: ", FALSE );
		print_socket_error( strerror(errno), TRUE );
	}
	else
	{
		/*say who the server is connected to*/
		printf( "(accept_socket)Connected to client with address %s\n", inet_ntoa(client.sin_addr) );

		/*char *msg = "hi\0";
		int len = 3;
		TCP_send( new_sock, msg, len, 0 );*/
	}
}

int TCP_send( socket_t *sock, char indicator, const char *data )
{
	int return_val;
	char *to_send = NULL;

	/*copy indicator, data and - if not already there - a '\n' to *to_send*/
	if( data[strlen(data)-1] != '\n' )
	{
		to_send = calloc( strlen(data) + 2, sizeof(char) );	//indicator + strlen + '\n' (no '\0')
		*to_send = indicator;
		strncpy( to_send+1, data, strlen(data) );
		to_send[strlen(data)+1] = '\n';
		return_val = send( *sock, to_send, 1+strlen(data)+1, 0 );
		free(to_send);
	}
	else
	{
		to_send = calloc( strlen(data) + 1, sizeof(char) );	//indicator + strlen (no '\0')
		*to_send = indicator;
		strncpy( to_send+1, data, strlen(data) );
		return_val = send( *sock, to_send, 1+strlen(data), 0 );
		free(to_send);
	}

	/*check*/
	if( return_val == -1 )
	{
		print_socket_error( "Error sending data: ", FALSE );
		print_socket_error( strerror(errno), TRUE );
	}
	else if( (unsigned)return_val != (1+strlen(data)) )
	{
		print_socket_error( "Not all data was sent: ", FALSE );
		print_socket_error( strerror(errno), TRUE );
	}
	return return_val;

}

/*receives message. Length calculated is equal to strlen(message). one extra byte is added for termination, containing NULL*/
int TCP_recv( socket_t *sock, char **data, status *readable )
{
	/*if readable pointer is NULL, do nothing*/
	if( readable != NULL )
	{
		fd_set readfds;
		struct timeval timeout;
		int highestfd;

		/*check if read would block*/
		FD_ZERO( &readfds );
		FD_SET( *sock, &readfds );
		reset_timeout( &timeout );
		highestfd = *sock;
		my_select( highestfd+1, &readfds, NULL, NULL, &timeout );

		/*if read will not block, start the receiving action*/
		if( FD_ISSET( *sock, &readfds ) )
		{
			int to_return;		//return value of recv() and this function
			char *length_str = NULL;

			*readable = SUCCESS;

			/*receive first 4 bytes, which indicate message length as a character (example: "4323")*/
			length_str = calloc( 4, sizeof(char) );
			to_return = recv( *sock, length_str, 4, 0 );
			printf("TCP_recv() length: %s\n", length_str );

			/*error*/
			if( to_return == -1 )
			{
				print_socket_error( "(TCP_recv)Error receiving length: ", FALSE );
				print_socket_error( strerror(errno), TRUE );
			}
			/*client gone*/
			else if( to_return == 0 )
			{
				print_socket_error( "(TCP_recv)Looks like the other end is gone: ", FALSE );
				print_socket_error( strerror(errno), TRUE );
			}
			/*filled length_str*/
			else
			{
				int length = 0;

				/*calculate length of following message (example: turn "4323" into 4323)*/
				double exponent = 3;
				for( int i = 0; i < 4; i++ )
				{
					length += ( pow( 10, exponent ) * ( length_str[i] - 48 ) );
					exponent--;
				}

				/*allocate enough space to save the message in*/
				*data = calloc( length+1, sizeof(char) );

				/*recv() actual message*/
				to_return = recv( *sock, *data, length, 0 );
				printf("TCP_recv() data: %s\n", *data );

				/*error*/
				if( to_return == -1 )
				{
					print_socket_error( "(TCP_recv)Error receiving data: ", FALSE );
					print_socket_error( strerror(errno), TRUE );
				}
				/*client gone*/
				else if( to_return == 0 )
				{
					print_socket_error( "(TCP_recv)Looks like the other end is gone: ", FALSE );
					print_socket_error( strerror(errno), TRUE );
				}
				/* *data filled*/
				else
				{
					*( *data + length ) = '\0';
				}
			}

			return to_return;
		}
		/*read would block*/
		else
		{
			*readable = FAILURE;
			return -1;
		}
	}//end readable != NULL

	print_socket_error( "(TCP_recv)Readable pointer is 0, not trying to receive.", TRUE );
	return -1;		//nothing was done because readable pointer is not given
}

void close_socket( socket_t *sock )
{
	if( close(*sock) != 0 )
	{
		print_socket_error( "Error closing socket: ", FALSE );
		print_socket_error( strerror(errno), TRUE );
	}
}
