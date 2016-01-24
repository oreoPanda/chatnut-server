/* This file contains all functions needed to handle user logins, logouts and registrations.*/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>   //for mkdir()

#include "messaging.h"
#include "users.h"
#include "socketprx.h"	//for SUCCESS and FAILURE

FILE *userlist = NULL;
char *raw_data = NULL;

int users = 0;  //number of loaded users (all users are always loaded)
char **usernames = NULL;
char **passwords = NULL;

/*WARNING: This code uses text passwords, no hashing involved :D	
	(for C++ look at libcryptopp), maybe try glib hashing for C*/

extern int init_server_directory(void)
{
    if( chdir(getenv("HOME")) != 0 )
    {
        fprintf( stderr, "Error switching into your home directory: %s\n", strerror(errno) );
        return 1;
    }
    if( mkdir( ".chatnut-server", 0755 ) != 0 )
    {
        if( errno != EEXIST )
        {
            fprintf( stderr, "Error creating directory .chatnut-server in your home directory: %s\n", strerror(errno) );
            return 1;
        }
    }
    if( chdir(".chatnut-server") != 0 )
    {
        fprintf( stderr, "Error switching to .chatnut-server in your home directory: %s\n", strerror(errno) );
        return 1;
    }
    
    return 0;
}
 
/*This function loads username and passwords of all registered users into char **usernames and char **passwords*/
//TODO check and debug
static int load_all_users(void)
{
    int file_length = 0;

    userlist = fopen( USERFILE, "r" );
    if( userlist == NULL )
    {
        perror(""); //prints error message associated to last errno
        return FILE_OPEN_ERROR;
    }
    else
    {
        //get file length
        fseek( userlist, 0, SEEK_END );
        file_length = ftell( userlist );
        rewind(userlist);
        
        //set up data buffer
        raw_data = calloc( file_length+1, sizeof(char) );   //one extra for termination
        
        //read from file
        fread( raw_data, sizeof(char), file_length, userlist );
        raw_data[file_length] = 0;
        
        //close file
        fclose(userlist);
        
        //crunch the data
        int i = 0;
        int usernameLen = 0;
        int passwordLen = 0;
        while( raw_data[i] != 0 )
        {
            //grab a username
            int username_pos = i;   //save the position of the username
            while( raw_data[i] != '\n' )
            {
                i++;
            }
            usernameLen = i-username_pos;   //calculate length of the username
            usernames = realloc( usernames, (users+1)*sizeof(char *) );     //add an element
            *(usernames+users) = calloc( usernameLen+1, sizeof(char) );    //memory for the username + '\0'
            strncpy( *(usernames+users), raw_data+username_pos, usernameLen );
            *( *(usernames+users) + usernameLen ) = 0;       //terminate
            
            i++;        //skip past the newline
            
            //grab a password
            int password_pos = i;   //save the position of the password
            while( raw_data[i] != '\n' )
            {
                i++;
            }
            passwordLen = i-password_pos;
            passwords = realloc( passwords, (users+1)*sizeof(char *) );
            *(passwords+users) = calloc( passwordLen+1, sizeof(char) );
            strncpy( *(passwords+users), raw_data+password_pos, passwordLen );
            *( *(passwords+users) + passwordLen ) = 0;
            
            i++;        //skip past the newline
            
            users++; //advance number of loaded users
        }
        //terminate the username and password arrays
        usernames = realloc( usernames, users+1*sizeof(char *) );
        passwords = realloc( passwords, users+1*sizeof(char *) );
        *(usernames+users) = NULL;
        *(passwords+users) = NULL;
        
        //free data buffer
        free(raw_data);
    }
    return 0;
}

extern int login( char *username, char *password )
{
	int position;

	if( find_user( username, &position ) == FAILURE )
	{
		return LOGIN_FAILURE;
	}
	else
	{
		//check if the given password matches the one that was loaded
		if( strcmp( *(passwords+position), password ) == 0 )
		{
			return LOGIN_SUCCESS;
		}
	}

	//find_user didn't fail but password was wrong
	return LOGIN_FAILURE;
}

//TODO check and debug
extern int find_user( char *username, int *index )
{
	//load registered users from file if this hasn't already been done
	if( users == 0 )
	{
		if( load_all_users() == FILE_OPEN_ERROR )
		{
			return FAILURE;
		}
	}

	//scan through loaded users to check if the requested one exists
	for( int i = 0; i < users; i++ )
	{
		printf( "(find_user)i: %d, *(usernames+i): %s, username: %s\n", i, *(usernames+i), username );
		if( strcmp( *(usernames+i), username ) == 0 )
		{
			if(index)
			{
				*index = i;
			}
			return SUCCESS;
		}
	}

	//if this line is reached, no user with the given name was found
	return FAILURE;
}

//TODO check and debug
extern int add_user( char *username, char *password )
{   
	//open file
	char *path = NULL;
	int pathlen = 0;
	char *homedir = getenv("$HOME");

	pathlen = strlen(homedir) + 1 + strlen(".message_server") + 1 + strlen(USERFILE) + 1;
	path = calloc( pathlen, sizeof(char) );//1 = '/' || NULL
	strncpy( path, homedir, strlen(homedir) + 1 );
	strncat( path, "/", 1 );
	strncat( path, ".message_server", 15 );
	strncat( path, "/", 1 );
	strncat( path, USERFILE, strlen(USERFILE) );

	//append the data to the file
	userlist = fopen( path, "a" );
	if( userlist == NULL )
	{
		perror(""); //prints error message associated to last errno
		return REGISTRATION_FAILURE;
	}

	fprintf( userlist, "%s\n", username );
	fprintf( userlist, "%s\n", password );
	fclose(userlist);

	//add element to char **usernames and char **passwords
	usernames = realloc( usernames, users+1*sizeof(char *) );
	passwords = realloc( passwords, users+1*sizeof(char *) );
	//write the data into the arrays (might not work this way, use strncpy?)
	usernames[users] = username;
	passwords[users] = password;

	return REGISTRATION_SUCCESS;
}

/* File format explanation:
 *      - for each block of two lines:
 *              - username in the first line
 *              - password in the second line
 */
