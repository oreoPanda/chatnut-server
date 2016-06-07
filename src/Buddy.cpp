/*Buddy.cpp*/

/*Copyright (C) 2016 Jonas Fuglsang-Petersen*/

/*This file is part of chatnut-server.*/

/*chatnut-server is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

chatnut-server is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with chatnut.  If not, see <http://www.gnu.org/licenses/>.*/

#include "Buddy.h"

namespace networking
{

	Buddy::Buddy()
	:client(NULL), clientname("")
	{

	}

	Buddy::~Buddy()
	{

	}

	/*Set a pointer to the client object of buddy and an iterator to an object of reverselist inside the client*/
	void Buddy::set_client(Client * const cli)
	{
		this->client = cli;
	}

	void Buddy::set_name(std::string const & name)
	{
		this->clientname = name;
	}

	void Buddy::set_iter(std::list<Buddy>::iterator const & i)
	{
		this->iter = i;
	}

	/*returns pointer to the client*/
	Client * Buddy::get_client()
	{
		return this->client;
	}

        /*returns a reference to the const clientname*/
	std::string const & Buddy::get_name() const
	{
		return this->clientname;
	}

        /*returns a reference to the const iter*/
	std::list<Buddy>::iterator const & Buddy::get_iter() const
	{
		return this->iter;
	}

}	/* namespace networking */
