/*Getch.cpp*/

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

#include "Getch.h"

static struct termios new_io;
static struct termios old_io;

/*set terminal to cbreak mode	*
 *set flags ECHO and ICANON to 0*/
int cbreak(int fd)
{
	/*save previous terminal state*/
	if( tcgetattr(fd, &old_io) == -1 )	//check if it worked
	{
		return -1;
	}
	new_io = old_io;			//copy old state to new state

	/*change flags for new state*/
	new_io.c_lflag = new_io.c_lflag & ~(ECHO|ICANON);
	new_io.c_cc[VMIN] = 0;		//minimum 0 bytes, so getch doesn't wait until a key is pressed
	new_io.c_cc[VTIME]= 2;		//2 tenths of a second timeout, after that getch stops listening for key input

	/*set cbreak mode*/
	if( tcsetattr(fd, TCSAFLUSH, &new_io) == -1 )//check if it worked
	{
		return -1;
	}
	return 1;
}

/*the getch() function*/
int getch(void)
{
	int c;

	if( cbreak(STDIN_FILENO) == -1)	//check if cbreak() actually worked
	{
		return -1;
	}
	c = getchar();			//but with terminal in cbreak mode

	/*reset terminal to previous state*/
	tcsetattr( STDIN_FILENO, TCSANOW, &old_io );

	return c;		//if everything worked, return the character that was typed in
}
