/*getch.h*/

#include <stdio.h>		//for getchar();
#include <termios.h>		//for setting terminal flags
#include <unistd.h>

extern int cbreak(int fd);
extern int getch(void);
