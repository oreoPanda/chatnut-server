/*
 * Buddy.cpp
 *
 *  Created on: Feb 7, 2016
 *      Author: i_fly_gliders
 */

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
	void Buddy::set(Client * const cli)
	{
		this->client = cli;
	}

	void Buddy::set(std::string const & name)
	{
		this->clientname = name;
	}

	void Buddy::set(std::forward_list<Buddy>::iterator const & i)
	{
		this->iter = i;
	}

	/*returns a reference to the const name of this object*/
	std::string const & Buddy::get_name() const
	{
		return this->clientname;
	}

	Client * Buddy::get_client()
	{
		return this->client;
	}

	std::forward_list<Buddy>::iterator const & Buddy::get_iter() const
	{
		return this->iter;
	}

}	/* namespace networking */
