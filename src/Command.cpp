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
	:iscmd( str[0] == '/' ? true:false ), current(cur), command(str.substr(1,str.find_first_of(' ')-1) ), actionlist(actionsptr)
	{
		if(this->iscmd)
		{
			size_t begin_pos;
			size_t space_pos;

			/*jump behind command that has already been stored*/
			begin_pos = str.find_first_of(' ') + 1;

			/*strip argument TODO support for multiple arguments*/
			space_pos = str.find_first_of(' ', begin_pos);
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

	void Command::evaluate() const
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
	/*stores the names of the requested buddies in buddyvector inside the client*/
	/*creates an Action with an action receiver
	 * and an iterator pointing to a buddy object inside the buddylist of the sender of the command*/
	void Command::who_handle() const
	{
		/*scan through arguments*/
		/*TODO for multiple arguments: uncomment and check what exception to catch*/
		int i = 0;

		std::string argument = arguments.at(i);
		std::forward_list<networking::Buddy>::iterator const iter = this->current->add_buddy(argument);
		action::Action a(argument, iter);
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
