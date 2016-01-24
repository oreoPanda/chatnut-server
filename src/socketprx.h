/*socketprx.h - for UNIX*/

#ifndef SOCKETPRX_H
#define SOCKETPRX_H

#include <errno.h>		//for errno used in error_exit()
#include <fcntl.h>		//for nonblocking sockets
#include <netinet/in.h>		//for struct sockaddr_in used in connect()
#include <stdio.h>		//standard I/O
#include <stdlib.h>		//for exit(EXIT_FAILURE) used in error_exit()
#include <sys/select.h>		//for select() in connect()
#include <sys/socket.h>		//for sockets
#include <string.h>		//for memset zero-filling and memcpy
#include <unistd.h>		//for close()
#include <arpa/inet.h>		//for network byte order conversions
#include <netdb.h>		//for gethostbyname()

#define socket_t int
#define CLIENT_MAX 2

typedef enum
{
	SUCCESS,
	FAILURE
}status;

typedef enum
{
	FALSE,
	TRUE
}boolean;

void reset_timeout(struct timeval *timeout);
void error_exit( char *error_message );
int create_socket( int af, int type, int protocol );
status bind_socket( socket_t *sock, unsigned long address, unsigned short port );
status listen_socket( socket_t *sock );
void connect_socket( socket_t *sock, char *server_addr, unsigned short port );
void my_select( int numfds, fd_set *readfds, fd_set *writefds, fd_set *exceptfds, struct timeval *timeout );
void accept_socket( socket_t *sock, socket_t *new_sock );
int TCP_send( socket_t *sock, char indicator, const char *data );
int TCP_recv( socket_t *sock, char **data, status *readable );
void cleanup(void);
void close_socket( socket_t *sock );

#endif	//SOCKETPRX_H
