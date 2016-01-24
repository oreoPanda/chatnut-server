/*message_server.c*/

/*http://beej.us/guide/bgnet/output/html/multipage/advanced.html#blocking*/
/*for a great starters select() tutorial*/

#include "messaging.h"
#include "getch.h"
#include "socketprx.h"
#include "users.h"

//FIXME check if FD_SET and other functions need error checks that I haven't done yet
//TODO check if empty messages crash the server, this is technically possible due to the way the server checks for\
 disconnected clients (by select() and a 0 returned from read() )	FIXME empty messages can't be sent, or? :D
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

const char conn_msg[CONN_MSG_LEN] = "Connected\n";

void print_error(char *error_message)
{
	fprintf( stderr, "%s\n", error_message );

	return;
}

/*add a client and return a pointer to it*/
struct client *add_client( void )
{
    struct client *pointer = NULL;

    if( start == NULL ) //if none there yet
    {
        if( (start = malloc( sizeof(struct client) ) ) == NULL )
        {
            print_error("(add_client)Error: Out of memory: ");

            return start;	//NULL
        }

        start->login_status = FALSE;
        memset( start->name, 0, NAMELEN );
        start->next = NULL;
        start->buddy = NULL;

        return start;	//not NULL
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
        	print_error("(add_client)Error: Out of memory: ");

			return pointer->next;	//NULL
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
            return buddy;	//NULL
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
            return buddy;	//NULL
        }
        buddy = pointer->next;		//go to new element
        buddy->next = NULL;			//terminate the new last element
    }

    return buddy;
}

/*deletes a client element, returns pointer to element behind the deleted element. DOES NOT close socket.*/
//FIXME make sure no clients with open sockets get deleted (close them before this function)
struct client *delete_client(struct client *toDelete)
{
	struct client *pointer = NULL;

	if(start)
	{
		if( start == toDelete )	 //first element is supposed to be deleted
		{
			printf( "(delete_client)Deleting first client.\n" );
			pointer = start->next;	  //works even when start->next = NULL, because then the new start is just NULL
			free(start);
			start = pointer;	//set start equal to the new start element
			return pointer;	   //return new first element (start or NULL)
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
					printf( "(delete_client)Deleting client.\n" );
					pointer->next = nextPointer->next;//bridge past nextPointer, could make next element of pointer NULL (last elem. found)
					free(nextPointer);
					return pointer->next;	//return the element behind the deleted one (nextPointer was deleted, so this could either be another client or NULL)
				}
				pointer = nextPointer;
			}
		}
	}
	else
	{
		return NULL;
	}
}

/*this function deletes all buddy objects of a client*/
/*call this function with current_buddy set to client->buddy*/
void delete_buddies( struct client *client, struct buddy *current_buddy )
{
	/*do something if there is at least one buddy*/
	if( current_buddy )			//this line is only relevant for the first call (the one that launches the recursion)
	{
		while( current_buddy->next )
		{
			delete_buddies( client, current_buddy->next );
		}

		free(current_buddy);
	}

	return;
}

void delete_buddy_references_to(struct client *client)
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
	//the "Connected" message is enough for a login request...
//TODO fully adapt this function to argc
/*evaluate commands and return an integer that will later be used to generate a reply for the client*/
commandreply eval_cmd( char *completeCommand, struct client *client, char **data )
{
	char *command = strtok( completeCommand, " " );//cut off command
	printf( "(eval_cmd)COMMAND: %s\n", command );
	int argc = 0;
	char **argv = NULL;

	/*copy arguments into argv, argv is not terminated but argc shows how many elements are in it*/
	char *arg = NULL;
	while( (arg = strtok( NULL, " " )) != NULL )	//cut off one argument at a time
	{
		argv = realloc( argv, sizeof(char *) * (argc+1) );
		*(argv+argc) = calloc( strlen(arg) + 1, sizeof(char) );
		strncpy( *(argv+argc), arg, strlen(arg) + 1 );	//save in correct position of argv
		argc++;
	}
	printf( "(eval_cmd)%d Arguments\n", argc );

	if( strcmp( command, "/help" ) == 0 )
	{
		return HELP;
	}

	else if( strcmp( command, "/list" ) == 0 )
	{
		return LIST;
	}

	//TODO make this be able to handle multiple arguments at once, for group chats
	//FIXME this doesn't work
	else if( strcmp( command, "/who" ) == 0 )
	{
		int buddy_found = FALSE;	//will be set true if this function finds a buddy with the given name

		if( argc == 0 )
		{
			printf( "(eval_cmd)No argument in /who...\n" );
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
						struct buddy *buddy_pointer = add_buddy(client);
						if( buddy_pointer == NULL )	 //just for testing TODO remove this after testing
						{
							return NOMEM;
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
			*data = calloc( strlen(argv[0])+1, sizeof(char) );
			strncpy( *data, argv[0], strlen(argv[0])+1 );
			return BUDDY_IS_SET;
		}
		else return BUDDY_NOT_EXIST;
	}

	//the opposite of /who
	else if( strcmp( command, "/unwho" ) == 0 )
	{
		delete_buddies( client, client->buddy );
		client->buddy = NULL;

		return BUDDY_IS_UNSET;	//TODO this might not be the best idea (when it still returned BUDDY_IS_SET\
								HAHAHAHAHA thanks j3f took me an hour to find out why pending_action_data was freed twice\
								(/unwho and /who)
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
			login_status = login( argv[0], argv[1] );
			if( login_status == LOGIN_SUCCESS )
			{
				//set name and login_status in the client structure
				client->login_status = TRUE;
				strncpy( client->name, argv[0], NAMELEN );
				//set *data to name so the username can be returned to client for further handling
				*data = calloc( strlen(argv[0])+1, sizeof(char) );
				strncpy( *data, argv[0], strlen(argv[0])+1 );
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

	//TODO lookup registered user command
	else if( strcmp( command, "/lookup" ) == 0 )
	{
		printf( "(eval_cmd) argc: %d\n", argc );
		if( argc != 1 )
		{
			return LOOKUP_FAILURE;
		}
		/*call find_user, if found return LOOKUP_SUCCESS and set *data*/
		else if( find_user( argv[0], NULL ) == SUCCESS )
		{
			*data = calloc( strlen(argv[0])+1, sizeof(char) );
			strncpy( *data, argv[0], strlen(argv[0])+1 );
			return LOOKUP_SUCCESS;
		}
		else
		{
			return LOOKUP_FAILURE;
		}
	}

	//else
	printf( "(eval_cmd)Unknown command: %s\n", command );
	return ERROR;
}

/*Sends an appropriate reply to the client.*/
void cmd_reply( commandreply value, struct client *client, char *data ) //value indicates what kind of command we're dealing with, that is how cmd_reply knows what reply to send
{
    char *reply;
    char indicator = value;

    switch(value)
    {
        case ERROR:
        {
        	reply = calloc( strlen(ERROR_STR)+1, sizeof(char) );
            strncpy( reply, ERROR_STR, strlen(ERROR_STR)+1 );
            break;
        }

        case HELP:
        {
        	reply = calloc( strlen(HELP_STR)+1, sizeof(char) );
            strncpy( reply, HELP_STR, strlen(HELP_STR)+1 );
            break;
        }
			
        /*case LIST://FIXME check if this causes funny output when a client's name is not set but added to reply
        {
            struct client *pointer = start;

            strncpy( reply, "These people are currently connected: ", MSG_LEN-2 );

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

            break;
        }*/

        case BUDDY_IS_SET:
        {
        	reply = calloc( strlen(data) + 1 + strlen(BUDDY_IS_SET_STR)+1, sizeof(char) );
        	strncpy( reply, data, strlen(data)+1 );
        	strncat( reply, " ", 1 );
			strncat( reply, BUDDY_IS_SET_STR, strlen(BUDDY_IS_SET_STR) );
            break;
        }

        case BUDDY_IS_UNSET:
        {
        	reply = calloc( strlen(BUDDY_IS_UNSET_STR)+1, sizeof(char) );
        	strncpy( reply, BUDDY_IS_UNSET_STR, strlen(BUDDY_IS_UNSET_STR)+1 );
        	break;
        }

        case BUDDY_NOT_EXIST:
        {
        	reply = calloc( strlen(BUDDY_NOT_EXIST_STR)+1, sizeof(char) );
        	strncpy( reply, BUDDY_NOT_EXIST_STR, strlen(BUDDY_NOT_EXIST_STR)+1 );
            break;
        }

        case LOGIN_SUCCESS:
        {
        	reply = calloc( strlen(data) + 1 + strlen(LOGIN_SUCCESS_STR)+1, sizeof(char) );
        	strncpy( reply, data, strlen(data)+1 );
        	strncat( reply, " ", 1 );
        	strncat( reply, LOGIN_SUCCESS_STR, strlen(LOGIN_SUCCESS_STR) );
            break;
        }

        case LOGIN_FAILURE:
        {
        	reply = calloc( strlen(LOGIN_FAILURE_STR)+1, sizeof(char) );
        	strncpy( reply, LOGIN_FAILURE_STR, strlen(LOGIN_FAILURE_STR)+1 );
            break;
        }

        case REGISTRATION_SUCCESS:
        {
        	reply = calloc( strlen(REGISTRATION_SUCCESS_STR)+1, sizeof(char) );
        	strncpy( reply, REGISTRATION_SUCCESS_STR, strlen(REGISTRATION_SUCCESS_STR)+1 );
            break;
        }

        case REGISTRATION_FAILURE:
        {
        	reply = calloc( strlen(REGISTRATION_FAILURE_STR)+1, sizeof(char) );
        	strncpy( reply, REGISTRATION_FAILURE_STR, strlen(REGISTRATION_FAILURE_STR)+1 );
            break;
        }

        case LOOKUP_SUCCESS:
        {
        	reply = calloc( strlen(data) + 1 + strlen(LOOKUP_SUCCESS_STR)+1, sizeof(char) );
        	strncpy( reply, data, strlen(data)+1 );
        	strncat( reply, " ", 1 );
        	strncat( reply, LOOKUP_SUCCESS_STR, strlen(LOOKUP_SUCCESS_STR) );
        	break;
        }

        case LOOKUP_FAILURE:
        {
        	reply = calloc( strlen(LOOKUP_FAILURE_STR)+1, sizeof(char) );
        	strncpy( reply, LOOKUP_FAILURE_STR, strlen(LOOKUP_FAILURE_STR)+1 );
        	break;
        }

        case NOARG:
        {
        	reply = calloc( strlen(NOARG_STR)+1, sizeof(char) );
        	strncpy( reply, NOARG_STR, strlen(NOARG_STR)+1 );
            break;
        }

        case NOMEM:
        {
        	reply = calloc( strlen(NOMEM_STR)+1, sizeof(char) );
        	strncpy( reply, NOMEM_STR, strlen(NOMEM_STR)+1 );
        	break;
        }

        default:
        {
            fprintf( stderr, "(cmd_reply)Error, unexpected command flag\n" );
            break;
        }
    }

    //send reply without block check, because reply HAS to be sent TODO this is a possible hang point if client leaves in a bad way
    printf( "(cmd_reply)Will send reply \"%s\"- with code: %d\n", reply, indicator );
    TCP_send( &client->sock, indicator, reply );

    free(reply);

    return;
}

/*message/command getting function. Returns FAILURE if client left*/
int handle_once( struct client *client )
{
	char *msg_buffer = NULL;
	status readable;

	/*check for new messages/commands*/
	int retval = TCP_recv( &client->sock, &msg_buffer, &readable );

	if( readable == SUCCESS )	//status is SUCCESS when read would not block and FAILURE when read would block
	{
		printf( "(handle_once)message: %s\n", msg_buffer );

		if( retval == 0 )//no message received, client gone
		{
			close_socket( &client->sock );
			printf("(handle_once)Client disconnected\n");
			return FAILURE;
		}

		/*new incoming data*/
		else if( retval != -1 )
		{
			/*data is command*/
			if( *msg_buffer == '/' )
			{
				char *data = NULL;	//data will contain username upon login or buddyname on lookup / buddy_is_set
				int eval_ret = eval_cmd( msg_buffer, client, &data );
				cmd_reply( eval_ret, client, data );
			}//end data is command

			/*data is message, send to all buddies*/
			else
			{
				struct buddy *buddy = client->buddy;
				while( buddy != NULL )
				{
					//no select() check here since message HAS to be sent, FIXME possible hang point
					printf( "(handle_once)Will send message %s to buddy.\n", msg_buffer );
					printf( "(handle_once)client->sock = %p\n", &client->sock );
					printf( "(handle_once)buddy->sock %p\n", buddy->sock );
					if( TCP_send( buddy->sock, MESSAGE, msg_buffer ) == -1 )
					{
						close_socket( &client->sock );
						printf( "(handle_once)Connection broken\n" );
						return FAILURE;
					}
					printf("(handle_once)sent\n");
					buddy = buddy->next;
				}
			}//end data is message
		}//end handle incoming data
	}//end status readable == SUCCESS

	return SUCCESS;
}

int main(void)
{
	socket_t hostSock;
	struct client *client = NULL;
	
	/*create a TCP socket*/
	if( (hostSock = create_socket( PF_INET, SOCK_STREAM, 0 )) >= 0 )
	{
		if( bind_socket( &hostSock, INADDR_ANY, 1234 ) == SUCCESS &&
			listen_socket(&hostSock) == SUCCESS )
		{

			/*main loop, handles connections and messaging*/
			client = start;
			do
			{
				if( client == NULL )   //empty, check for connection to fill it
				{
					if( check_incoming( &hostSock ) == SUCCESS )
					{
						/*fill with connection*/
						client = add_client();
						if(client)
						{
							accept_socket( &hostSock, &client->sock );
							printf( "(main)Fully established connection to a new client!\n");
							TCP_send( &client->sock, CONNECTED, conn_msg );
							client = client->next;
						}
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
					/*handle client, if it left delete it. Go on with next client*/
					if( handle_once(client) == FAILURE )
					{
						delete_buddy_references_to(client);
						client = delete_client(client);	 //delete_client returns pointer to client behind deleted one
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
			while(client)
			{
				delete_buddy_references_to(client);
				client = delete_client(client);	 //delete_client again returns pointer to client behind deleted one
				if(client)
				{
					client = client->next;
				}
			}
		}
		close_socket(&hostSock);
	}

	return 0;
}
