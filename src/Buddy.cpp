/*
 * Buddy.cpp
 *
 *  Created on: Feb 7, 2016
 *      Author: i_fly_gliders
 */

#include "Buddy.h"

namespace networking
{

	Buddy::Buddy(std::string const & name)
	:client(NULL), clientname(name)
	{

	}

	Buddy::~Buddy()
	{

	}

	void Buddy::set(Client const * const cli, std::forward_list<Buddy>::iterator const & i)
	{
		this->client = cli;
		this->iter = i;
	}

}	/* namespace networking */
