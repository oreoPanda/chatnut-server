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
