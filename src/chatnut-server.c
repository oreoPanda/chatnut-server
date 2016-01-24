/*message_server.c*/

/*http://beej.us/guide/bgnet/output/html/multipage/advanced.html#blocking*/
/*for a great starters select() tutorial*/

#include <stdlib.h>

#include "messaging.h"
#include "server.h"
#include "getch.h"
#include "socketprx.h"

//FIXME check that all error_exit points lead to clean exits, not connection-breaks
//FIXME check if FD_SET and other functions need error checks that I haven't done yet
//TODO this server can recognize closed connections but I'm not sure about broken connections
//TODO check for too many arguments? (especially /who)

struct buddy
{
    socket_t *sock;
    struct buddy *next;
};

struct client
{
    socket_t sock;
    int login_status;
    char name[NAMELEN];
    struct client *next;
    struct buddy *buddy;
};

struct client *start = NULL;

const char conn_msg[CONN_MSG_LEN] = "Connected";	//put here instead of header because it would be defined twice if header is included in two files

/*add a client and return a pointer to it*/
struct client *add_client( void )
{
    struct client *pointer = NULL;

    if( start == NULL ) //if none there yet
    {
        if( (start = malloc( sizeof(struct client) ) ) == NULL )
        {
            error_exit( "Error: Not enough memory.\n" );
        }

        start->login_status = FALSE;
        memset( start->name, 0, NAMELEN );
        start->next = NULL;
        start->buddy = NULL;
        return start;
    }
    else
    {
        pointer = start;
        while( pointer->next != NULL )  //scan to last element, allocate space for the element after that
        {
            pointer = pointer->next;
        }
        if( (pointer->next = malloc( sizeof(struct client) ) ) == NULL )
        {
            error_exit( "Error: Not enough memory.\n" );
        }
        pointer = pointer->next;	//switch to the new element

        pointer->login_status = FALSE;
        memset( pointer->name, 0, NAMELEN );
        pointer->next = NULL;
        pointer->buddy = NULL;
        return pointer;
    }
}

struct buddy *add_buddy( struct client *client )
{
    struct buddy *buddy = NULL;

    if( client->buddy == NULL )	 //no buddy yet
    {
        if( ( client->buddy = malloc( sizeof(struct buddy) ) ) == NULL )
        {
            error_exit( "Error: Not enough memory.\n" );
        }
        buddy = client->buddy;
        buddy->next = NULL;
    }

    else		//at least one buddy is set already
    {
        struct buddy *pointer = NULL;

        pointer = client->buddy;
        while( pointer->next != NULL )
        {
            pointer = pointer->next;		//go to last element
        }

        if( ( pointer->next = malloc( sizeof(struct buddy) ) ) == NULL )	//allocate space for new element
        {
            error_exit( "Error: Not enough memory.\n" );
        }
        buddy = pointer->next;		//go to new element
        buddy->next = NULL;			//terminate the new last element
    }

    return buddy;
}

/*deletes a client element and returns pointer to element before the deleted element. DOES NOT close socket.*/
//FIXME make sure no clients with open sockets get deleted (close them before this function)
struct client *delete_client( struct client *toDelete )
{
    struct client *pointer = NULL;

    if( start != NULL )
    {
        if( start == toDelete )	 //first element is supposed to be deleted
        {
           printf( "Deleting first client.\n" );
            pointer = start->next;	  //works even when start->next = NULL, because then the new start is just NULL
            free(start);
            start = pointer;	//set start equal to the new start element
            return pointer;	   //return new first element (start)
        }
        else
        {
            struct client *nextPointer = NULL;

            pointer = start;
            while( pointer->next != NULL )  //only if there are elements behind this
            {
                nextPointer = pointer->next;	//pointer will point to one element, nextPointer to the one behind it
                if( nextPointer	== toDelete	)//nextPointer can be deleted, pointer will be returned (the element in front of deleted one)
                {
                    printf( "Deleting other client.\n" );
                    pointer->next = nextPointer->next;//bridge past nextPointer, could make next element of pointer NULL (last elem. found)
                    free(nextPointer);
                    return pointer->next;		//return the element behind the deleted one (nextPointer was deleted, so this could either be another client or NULL)
                }
                pointer = nextPointer;
            }
        }
    }
    else
    {
        error_exit( "No client to delete.\n" );
    }
    exit(EXIT_FAILURE);	 //I'm useless, I'm just here to get rid of the compiler warning "control reaches end of non-void function"
}

void delete_buddy_references_to( struct client *client )
{
    struct client *client_pointer = start;

    //scan thru clients
    while( client_pointer != NULL )
    {
        struct buddy *buddy_start = client_pointer->buddy;
        struct buddy *buddy_pointer = NULL;

        if( buddy_start != NULL )
        {
            //scan thru buddies
            //check if socket pointer of first buddy points at the the socket of the client specified via function parameter
            if( buddy_start->sock == &client->sock )
            {
                buddy_pointer = buddy_start->next;
                free(buddy_start);
                buddy_start = buddy_pointer;
                buddy_pointer = NULL;
                client_pointer->buddy = buddy_pointer;
            }
            else
            {
                struct buddy *buddy_nextPointer = NULL;

                buddy_pointer = buddy_start;
                while( buddy_pointer->next != NULL )
                {
                    buddy_nextPointer = buddy_pointer->next;
                    if( buddy_nextPointer->sock == &client->sock )
                    {
                        buddy_pointer->next = buddy_nextPointer->next;
                        free(buddy_nextPointer);
                    }
                    buddy_pointer = buddy_nextPointer;
                }
            }
        }

        client_pointer = client_pointer->next;
    }
}

int check_incoming( socket_t *hostSock )
{
    fd_set readfds;
    struct timeval timeout;
    int highestfd;

    FD_SET( *hostSock, &readfds );
    reset_timeout( &timeout );
    highestfd = *hostSock;

    //FIXME timeouts with select() (and getch())
    my_select( highestfd+1, &readfds, NULL, NULL, &timeout );	//select() needs highest numbered fd + 1
    if( FD_ISSET(*hostSock, &readfds) )	//if a connection is waiting
    {
        return SUCCESS;
    }
    else
    {
        return FAILURE;
    }
}

//TODO check login command (here and in cmd_reply())
//TODO rethink login and logout mechanism, a server-side login-request should be useful upon connecting a client. /logout should disconnect the client.
//TODO fully adapt this function to argc
/*evaluate commands and return an integer that will later be used to generate a reply for the client*/
int eval_cmd( char *completeCommand, struct client *client )
{
    char *command = strtok( completeCommand, " " );//cut off command
    printf( "COMMAND: %s\n", command );
    int argc = 0;
    char **argv = NULL;

    //TODO this is totally unclear...
    char *arg = NULL;
    while( (arg = strtok( NULL, " " )) ) //cut off one argument at a time
    {
        printf( "Found an(other) argument.\n" );
        argv = realloc( argv, argc+1*sizeof(char *) );
        argv[argc] = calloc( strlen(arg), sizeof(char) );
        strncpy( argv[argc], arg, NAMELEN-1 );//save in arg //FIXME sigsegv in this line when one argument is given
        argv[argc][NAMELEN-1] = 0;	 //terminate the argument in case the name was longer than NAMELEN
        argc++;
    }

    if( strcmp( command, "/help" ) == 0 )
    {
        return HELP_WANTED;
    }

    else if( strcmp( command, "/list" ) == 0 )
    {
        return LIST;
    }

    else if( strcmp( command, "/name" ) == 0 )
    {
        if( argc == 0 )
        {
            return NOARG;
        }
        else    //takes only the first argument, ignores all others
        {
            strncpy( client->name, argv[0], NAMELEN );//save the first argument as client name
            return NAME_IS_SET;
        }
    }

    //TODO make this be able to handle multiple arguments at once? - might not be needed in graphics version... maybe...
    //FIXME this doesn't work
    else if( strcmp( command, "/who" ) == 0 )
    {
        int buddy_found = FALSE;	//will be set true if this function finds a buddy with the given name

        if( argc == 0 )
        {
            printf( "No argument in /who...\n" );
            return NOARG;
        }
        else
        {
            struct client *client_pointer = start;

            /*scan thru clients*/
            while( client_pointer != NULL )
            {
                /*for each client, compare with all arguments*/
                for( int i = 0; i < argc; i++ )
                {
                    if( strcmp(client_pointer->name, argv[i]) == 0 )//not using strncmp() because I can be sure the argument is terminated
                    {
                        //request a new buddy object
                        struct buddy *buddy_pointer = add_buddy( client );
                        if( buddy_pointer == NULL )	 //just for testing TODO remove this after testing
                        {
                                error_exit( "Fatal error in eval_cmd(), didn't receive buddy from add_buddy()\n" );
                        }
                        //set the buddy's connection sock in the buddy object
                        else
                        {
                                buddy_pointer->sock = &client_pointer->sock;
                                buddy_found = TRUE;
                        }
                    }
                }

                client_pointer = client_pointer->next;
            }
        }

        if( buddy_found )
        {
                return BUDDY_IS_SET;
        }
        else return BUDDY_NOT_EXIST;
    }

    //login command
    else if( strcmp( command, "/login" ) == 0 )
    {
        int login_status = LOGIN_FAILURE;

        if( argc != 2 )
        {
            return LOGIN_FAILURE;
        }
        else
        {
            login_status = find_user( argv[0], argv[1] );
            if( login_status == LOGIN_SUCCESS )
            {
                //set name and login_status in the client structure
                client->login_status = TRUE;
                strncpy( client->name, argv[0], NAMELEN );
                return LOGIN_SUCCESS;
            }
            else
            {
                return LOGIN_FAILURE;
            }
        }
    }

    //register command
    else if( strcmp( command, "/register" ) == 0 )
    {
        int registration_status = REGISTRATION_FAILURE;
        if( argc != 2 )
        {
            return REGISTRATION_FAILURE;
        }
        else
        {
            registration_status = add_user( argv[0], argv[1] );
            if( registration_status == REGISTRATION_SUCCESS )
            {
                return REGISTRATION_SUCCESS;
            }
            else
            {
                return REGISTRATION_FAILURE;
            }
        }
    }

    //logout command
    else if( strcmp( command, "/logout" ) == 0 )
    {
        client->login_status = FALSE;
    }

    else	//unknown command
    {
        printf( "Unknown command: %s\n", command );
        return ERROR;
    }
}

//FIXME if messages are cut off when reaching the client, they are longer than CMD_LEN and get cut off by the second-to-last statement of this function
//TODO check login command
void cmd_reply( int value, struct client *client ) //value indicates what kind of command we're dealing with, that is how cmd_reply knows what reply to send
{
    char reply[MSG_LEN];

    reply[0] = value;

    switch(value)
    {
        case ERROR:
        {
            strncpy( reply+1, "That command doesn't exist. Try `/help`.\0", MSG_LEN-2 );	//minus 2 because we started one byte in
            break;
        }

        case HELP_WANTED:
        {
            strncpy( reply+1, "Usage: [command] [space-separated arguments]\n"\
            "Commands:\n"\
                "\t/help\tshow this help\n"\
                "\t/name\tset your name\n"\
                "\t/who\tset buddies you want to talk to\n"\
                "\t/list\tshow a list of connected people\n"\
            "Arguments:\n"\
                "\tnames\0", MSG_LEN-2 );
            break;
        }
			
        case LIST://FIXME check if this causes funny output when a client's name is not set but added to reply
        {
            struct client *pointer = start;
            strncpy( reply+1, "These people are currently connected: ", MSG_LEN-2 );

            while( pointer != NULL )		//scan through all clients and add their names to the reply
            {
                strncat( reply, pointer->name, MSG_LEN-strlen(reply)-1 );
                if( pointer == client )	 //make a note if name is the own
                {
                    strncat( reply, " (me)", MSG_LEN-strlen(reply)-1 );
                }
                strncat( reply, ", ", MSG_LEN-strlen(reply)-1 );
                pointer = pointer->next;
            }
			
            strncat( reply, "\0", MSG_LEN-strlen(reply)-1 );

            break;
        }
	
        case NAME_IS_SET:
        {
            strncpy( reply+1, "Successfully (re)set name.\0", MSG_LEN-2 );
            break;
        }

        case BUDDY_IS_SET:
        {
            strncpy( reply+1, "Successfully (re)set buddies.\0", MSG_LEN-2 );
            break;
        }

        case BUDDY_NOT_EXIST:
        {
            strncpy( reply+1, "The buddy you requested does not exist. Try `/list`\0", MSG_LEN-2 );
            break;
        }

        case LOGIN_SUCCESS:
        {
            strncpy( reply+1, "Login successful.\0", MSG_LEN-2 );
            break;
        }

        case LOGIN_FAILURE:
        {
            strncpy( reply+1, "Login unsuccessful. Please try again.\0", MSG_LEN-2 );
            break;
        }

        case REGISTRATION_SUCCESS:
        {
            strncpy( reply+1, "Registration successful.\0", MSG_LEN-2 );
            break;
        }

        case REGISTRATION_FAILURE:
        {
            strncpy( reply+1, "Registration unsuccessful. Please try again.\0", MSG_LEN-2 );
            break;
        }

        case NOARG:
        {
            strncpy( reply+1, "This command requires an argument.\0", MSG_LEN-2 );
            break;
        }

        default:
        {
            fprintf( stderr, "Error, unexpected command flag\n" );
            break;
        }
    }

    reply[MSG_LEN-1] = 0;   //terminate string (strncpy doesn't do it when strings are too long)

    //send reply without block check, because reply HAS to be sent TODO this is a possible hang point if client leaves in a bad way
    TCP_send( &client->sock, reply, strlen(reply)+1, 0 );
}

/*message/command getting function. Returns FAILURE if client left*/
int handle_once( struct client *client )
{
    printf( "inside handle_once, operating on client with socket %d\n", client->sock );
    char *msg_buffer = malloc( MSG_LEN*sizeof(char) );

    /*check for new messages/commands*/
    int status;
    printf( "inside handle_once, before TCP_recv\n" );
    int retval = TCP_recv( &client->sock, msg_buffer, MSG_LEN, 0, &status );
    printf( "inside handle_once, after TCP_recv\n" );
    if( status == SUCCESS )	//status is SUCCESS when read would not block and FAILURE when read would block
    {
        if( retval == 0 )//no message received, client gone
        {
            close_socket( &client->sock );
            printf("Client disconnected\n");
            return FAILURE;
        }

        /*new incoming data*/
        else if( retval != -1 )
        {
            /*data is command*/
            if( *msg_buffer == '/' )
            {
                int eval_ret = eval_cmd( msg_buffer, client );
                cmd_reply( eval_ret, client );
            }//end data is command

            /*data is message, send to all buddies*/
            else
            {
                struct buddy *buddy = client->buddy;
                while( buddy != NULL )
                {
                    //no select() check here since message HAS to be sent, FIXME possible hang point
                    if( TCP_send( buddy->sock, msg_buffer, MSG_LEN, 0 ) == -1 )
                    {
                        close_socket( &client->sock );
                        printf( "Connection broken\n" );
                        return FAILURE;
                    }
                    buddy = buddy->next;
                }
            }//end data is message
        }//end handle incoming data
    }//end status SUCCESS

    return SUCCESS;
}

int main(void)
{
	socket_t hostSock;
	
	/*create a TCP socket*/
	hostSock = create_socket( PF_INET, SOCK_STREAM, 0 );
	atexit(cleanup);
	bind_socket( &hostSock, INADDR_ANY, 1234 );
	listen_socket(&hostSock);

	/*main loop, handles connections and messaging*/
	struct client *client = start;
	do
	{
		if( client == NULL )   //empty, check for connection to fill it
		{
			printf( "Client is NULL\n" );
			if( check_incoming( &hostSock ) == SUCCESS )
			{
				printf( "Inbound\n" );
				/*fill with connection*/
				client = add_client();
				printf( "Will now accept\n");
				accept_socket( &hostSock, &client->sock );
				printf( "Accepted!\n");
				TCP_send( &client->sock, conn_msg, CONN_MSG_LEN, 0 );
				client = client->next;
				continue;
			}
			else
			{
				client = start; //end of list was reached and could not be filled, so next time the while loop will start over again
				continue;	   //element is empty and could not be filled
			}
		}
		else		//client element is not NULL
		{
			//TODO this is not checked completely
			if( handle_once(client) == FAILURE )
			{
				printf( "handle_once returned FAILURE\n" );
				delete_buddy_references_to(client);
				client = delete_client(client);	 //delete_client returns a pointer to the client in front of the deleted one
			}
			else
			{
				client = client->next;
			}
		}
	}
	while( getch() != 'q' );	//do one action with one client, then check this again

	/*upon quitting*/
	//delete clients and buddy references
	client = start;
	while( client != NULL )
	{
		delete_buddy_references_to( client );
		client = delete_client(client);	 //delete_client again returns a pointer to the client in front of the deleted one
		if( client != NULL )
		{
			client = client->next;
		}
	}
	
	close_socket(&hostSock);

	return 0;
}
