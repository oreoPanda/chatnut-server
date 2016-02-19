/*
 * Command.cpp
 *
 *  Created on: Feb 11, 2016
 *      Author: i_fly_gliders
 */

#include "messaging.h"

namespace messaging
{

	Command::Command(std::string const str, networking::Client * const cur, std::forward_list<action::Action> *actionsptr)
	:iscmd( str[0] == '/' ? true:false ), current(cur), actionlist(actionsptr)
	{
		if(this->iscmd)
		{
			size_t begin_pos = 1;
			size_t space_pos;

			/*strip command*/
			space_pos = str.find_first_of(' ');
			this->command = str.substr(begin_pos, space_pos-1);
			begin_pos += 1;

			/*strip arguments*/
			space_pos = str.find_first_of(' ');
			if(space_pos != std::string::npos)
			{
				this->arguments.push_back(str.substr(begin_pos, space_pos-1) );
			}
		}
	}

	Command::~Command()
	{

	}

	bool Command::isCommand() const
	{
		return this->iscmd;
	}

	void Command::start_eval() const
	{
		if(this->command.compare("/who") == 0)
		{
			this->who_handle();
		}
		else if(this->command.compare("/unwho") == 0)
		{
			this->unwho_handle();
		}
		else if(this->command.compare("/lookup") == 0)
		{
			lookup_handle();
		}
		else if(this->command.compare("/login") == 0)
		{
			login_handle();
		}
		else if(this->command.compare("/logout") == 0)
		{
			logout_handle();
		}
		else
		{
			this->unknown_handle();
		}
	}

	/*Handles the /who command (surprise! :o)*/
	/*stores the names of the requested buddies in buddyvector inside the client and creates an Action with names of requested buddies*/
	void Command::who_handle() const
	{
		action::Action temp();
		/*scan through arguments*/
		/*TODO for multiple arguments: uncomment and check what exception to catch*/
		int i = 0;
		do
		{
			//try
			//{
				std::string argument = arguments.at(i);
			//}
			//catch
			//{
				//break;
			//}
			//this->current->add_buddy(argument);		TODO FIXME TODO uncomment and pass an action pointer
			break;		//TODO for multiple arguments: delete this line
			//i++;
		}
		while(true);
	}

	void Command::unwho_handle() const
	{

	}

	void Command::lookup_handle() const
	{

	}

	void Command::login_handle() const
	{

	}

	void Command::logout_handle() const
	{

	}

	void Command::unknown_handle() const
	{
		std::string reply = ERROR + ERROR_STR;

		current->Send(reply);
	}

} /* namespace messaging */
