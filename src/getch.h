/*getch.h*/

#ifndef GETCH_H
#define GETCH_H

#include <stdio.h>		//for getchar();
#include <termios.h>		//for setting terminal flags
#include <unistd.h>

int cbreak(int fd);
int getch(void);

#endif	//GETCH_H
