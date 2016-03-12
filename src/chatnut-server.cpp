/*chatnut-server.cpp*/

/*http://beej.us/guide/bgnet/output/html/multipage/advanced.html#blocking*/
/*for a great starters select() tutorial*/

#include <iostream>
#include <list>
#include <string>

#include "Getch.h"
#include "LogWriter.h"
#include "messaging.h"
#include "networking.h"
#include "Storage.h"
#include "Userlist.h"

using namespace fileio;
using namespace messaging;
using namespace networking;

//TODO this server can recognize closed connections but I'm not quite sure about broken connections
//TODO why can two instances of this server run at the same time without complaining about port or address already in use?

/*delete a client (the one given in argument) and return the previous one or NULL*/
Client * deleteClient(Client * current)
{
	/*Delete current client*/
	Client *temp = current;
	/*there are clients left in circle*/
	if( current->Next() != current )
	{
		current = current->Next();
	}
	/*the client that is deleted is the last one in the circle*/
	else
	{
		current = NULL;
	}
	delete temp;
	/*jump back to previous so that jumping to next won't leave anyone out*/
	if(current)
	{
		current = current->Prev();
	}

	return current;
}

int main(void)
{
	Client * current = NULL;
	LogWriter logger(std::cerr, std::clog);
	Storage storage(getenv("HOME"), logger);
	Userlist userlist(logger);

	/*only go on if directory initialization was successful*/
	if(storage.init_success() == false)
	{
		return 1;
	}

	/*Initialize host class*/
	Horst horst(1234, 100, logger);	//TODO Caution: 100 clients is a low limit
	if( horst.init_success() )
	{
		/*Create action list*/
		std::forward_list<Action> actions;
		/*main loop*/
		do
		{
			/*check for new connection request*/
			if( horst.check_incoming() )
			{
				struct sockaddr_in addr;
				socket_t client_sock;

				client_sock = horst.accept_connection(addr);
				if( client_sock != -1 )
				{
					/*Add first client*/
					if(!current)
					{
						current = new Client(client_sock, addr, NULL, &logger);
						/*Send the client the Connected reply along with a short message*/
						Command welcome_cmd(current, userlist, logger);
					}
					/*Add client behind current*/
					else
					{
						new Client(client_sock, addr, current, &logger);
						/*Send the client the Connected reply along with a short message*/
						Command welcome_cmd(current->Next(), userlist, logger);
					}
				}//end connection accepted
			}//end horst.check_incoming()

			/*Check if the current client sent any messages or commands*/
			if(current && current->get_Connected() )
			{
				/*handle actions*/
				if(current->get_Login() == true)
				{
					current->handle_actions(actions);
				}

				/*check for a new message*/
				if(current->check_incoming())
				{
					/*get new message*/
					std::string msg;
					if( current->get_message(msg) )
					{
						Command cmd(msg, current, &actions, userlist, logger);
						if(cmd.isCommand() )
						{
							cmd.evaluate();
						}
						else
						{
							Message message(msg, current, logger);
							message.send_or_store();
						}
					}
				}
			}//end current && current->get_Connected()

			/*if the client is no longer connected (Connected message failed or he is actually gone) delete it*/
			if(current && !current->get_Connected() )
			{
				current = deleteClient(current);
			}

			/*Move on to the next client*/
			if(current)
			{
				current = current->Next();
			}
		}
		while( getch() != 'q' );

		/*delete all clients*/
		while(current)
		{
			/*Delete current*/
			current = deleteClient(current);
		}
	}

	return 0;
}
