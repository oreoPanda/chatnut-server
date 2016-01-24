/*socketprx.h - for UNIX*/

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
	//TODO do I need sys/types.h?

#define socket_t int
#define SUCCESS 0
#define FAILURE 1
#define CLIENT_MAX 2

extern void reset_timeout(struct timeval *timeout);
extern void error_exit( char *error_message );
extern int create_socket( int af, int type, int protocol );
extern void bind_socket( socket_t *sock, unsigned long address, unsigned short port );
extern void listen_socket( socket_t *sock );
extern void connect_socket( socket_t *sock, char *server_addr, unsigned short port );
extern void my_select( int numfds, fd_set *readfds, fd_set *writefds, fd_set *exceptfds, struct timeval *timeout );
extern void accept_socket( socket_t *sock, socket_t *new_sock );
extern int TCP_send( socket_t *sock, const char *data, int len, int flags );
extern int TCP_recv( socket_t *sock, char *data, int len, int flags, int *status );
extern void cleanup(void);
extern void close_socket( socket_t *sock );
