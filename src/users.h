/*server.h*/

#ifndef USERS_H
#define USERS_H

#define USERFILE "users"

extern int init_server_directory(void);
extern int login( char *username, char *password );
extern int find_user( char *username, int *index );
extern int add_user( char *username, char *password );

#endif	//USERS_H
