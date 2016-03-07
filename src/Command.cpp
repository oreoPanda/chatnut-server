/*
 * Command.cpp
 *
 *  Created on: Feb 11, 2016
 *      Author: i_fly_gliders
 */

#include "messaging.h"

namespace messaging
{

        /*this Constructor is only used when a new client connects, to send the Connected message TODO move to message class*/
	Command::Command(networking::Client * const cur)
	:iscmd(false), current(cur), command(""), actionlist(NULL), reader(NULL)
	{
		this->replies.push_back("Connected\n");
		connected_handle();
	}

	Command::Command(std::string const str, networking::Client * const cur, std::forward_list<networking::Action> *actionsptr, fileio::StorageReader * const r)
	:iscmd( str[0] == '/' ? true:false ), current(cur), command(str.substr(1,str.find_first_of(' ')-1) ), actionlist(actionsptr), reader(r)
	{
		if(this->iscmd)
		{
			size_t begin_pos;
			size_t space_pos;

			/*jump behind command that has already been stored*/
			begin_pos = str.find_first_of(' ') + 1;

			do
			{
				space_pos = str.find_first_of(' ', begin_pos);
				this->arguments.push_back(str.substr(begin_pos, space_pos-begin_pos) );
				begin_pos = space_pos+1;
			}
			while(space_pos != std::string::npos);
		}

		init_replies();
	}

	Command::~Command()
	{

	}

	/*fill the replies vector with strings describing the return value sent to client after handling its requested command*/
	void Command::init_replies()
	{
		this->replies.push_back("Connected\n");
		this->replies.push_back("Usage: [command] [space-separated arguments]\n\
				Commands:\n\
					\t/help\tshow this help\n\
					\t/name\tset your name\n\
					\t/who\tset buddies you want to talk to\n\
					\t/list\tshow a list of connected people\n\
				Arguments:\n\
					\tnames\n");
		this->replies.push_back("Successfully set buddy.\n");
		this->replies.push_back("Successfully unset buddy.\n");		//TODO change message when multiple buddies can be set
		this->replies.push_back("The buddy you requested does not exist.\n");
		this->replies.push_back("Login successful.\n");
		this->replies.push_back("Login unsuccessful. Please try again.\n");
		this->replies.push_back("Registration successful.\n");
		this->replies.push_back("Registration unsuccessful. Please try again.\n");
		this->replies.push_back("User found.\n");
		this->replies.push_back("That user doesn't exist. Please try again.\n");
		this->replies.push_back("");
		this->replies.push_back("This command requires an argument.\n");		//placeholder, MESSAGE gets its own message string each time
		this->replies.push_back("The server doesn't have enough memory left for the requested operation.\n");
		this->replies.push_back("That command doesn't exist. Try `/help`\n.");

		return;
	}

	bool Command::isCommand() const
	{
		return this->iscmd;
	}

	void Command::evaluate() const
	{
		if(this->command.compare("who") == 0)
		{
			this->who_handle();
		}
		else if(this->command.compare("unwho") == 0)
		{
			this->unwho_handle();
		}
		else if(this->command.compare("lookup") == 0)
		{
			lookup_handle();
		}
		else if(this->command.compare("login") == 0)
		{
			login_handle();
		}
		else if(this->command.compare("logout") == 0)
		{
			logout_handle();
		}
		else
		{
			this->unknown_handle();
		}
	}

	void Command::connected_handle() const
	{
		construct_reply(CONNECTED);
	}

	/*Handles the /who command (surprise! :o)*/
        /*stores the names of the requested buddies in buddylist inside the client*/
        /*creates an Action (see networking.h for Action description)*/
	void Command::who_handle() const
	{
            /*TODO multiple arguments*/
                //get one argument at the time
                std::string argument = arguments.at(0);

                //store the argument in a new object of buddylist inside client
                std::forward_list<networking::Buddy>::iterator const iter = this->current->add_buddy(argument);

                //create the action
		networking::Action a(this->current, argument, iter);
		actionlist->push_front(a);

		/*TODO send an adequate reply*/
	}

	void Command::unwho_handle() const
	{

	}

	void Command::lookup_handle() const
	{

	}

	void Command::login_handle() const
	{
                bool login_status = false;
            
		if( arguments.size() == 2 )
		{
			bool password_correct = reader->check_password_for(this->arguments.at(0), this->arguments.at(1) );
			if(password_correct)
			{
				this->current->set_Login(true);
				construct_reply(LOGIN_SUCCESS);
                                login_status = true;
			}
			else
			{
				construct_reply(LOGIN_FAILURE);
                                login_status = false;
			}
		}
		else
		{
			//TODO check the order of sending these two commands (depends a little on implementation in the client)
			construct_reply(LOGIN_FAILURE);
			construct_reply(NOARG);
                        login_status = false;
		}
		
		if(login_status == true)
                {
                    /*read messages from temporary storage and send them to current*/
                    reader->set_receiver(arguments.at(0) );
                    //reader: get filelist
                    //for each file:
                        //reader: switch to receiver dir
                        //reader: read all messages from one file into vector or array (each line one object)
                        //this: for each object of vector
                            //current: send message to 
                        //reader: switch to ../
                    //move to next
                }
	}

	void Command::logout_handle() const
	{

	}

	void Command::unknown_handle() const
	{
		construct_reply(ERROR);
	}

	void Command::construct_reply(reply indic) const
	{
		char * indicator_c = static_cast<char *>(calloc(1, sizeof(char) ) );
		indicator_c[0] = indic;
		std::string const indicator = indicator_c;

		/*set up the message TODO check that it has the right length*/
		std::string const str = indicator + replies.at(indic-32);

		/*send the message*/
		current->Send(str);
	}

} /* namespace messaging */
