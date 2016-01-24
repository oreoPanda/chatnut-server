/* This file contains all functions needed to handle user logins, logouts and registrations.*/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "messaging.h"
#include "server.h"

FILE *userlist = NULL;
char *raw_data = NULL;

int users = 0;  //number of loaded users (all users are always loaded)
char **usernames = NULL;
char **passwords = NULL;

/*WARNING: This code uses text passwords, no hashing involved :D	
	(for C++ look at libcryptopp)*/

/*This function loads username and passwords of all registered users into char **usernames and char **passwords*/
//TODO check and debug
int load_all_users(void)
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

//TODO check and debug
int find_user( char *username, char *password )
{
    //load registered users from file if this hasn't already been done
    if( users == 0 )
    {
        if( load_all_users() == FILE_OPEN_ERROR )
        {
            return LOGIN_FAILURE;
        }
    }
    
    //scan through loaded users to check if the requested one exists
    int i = 0;      //declared here because it is needed after the for-loop
    for( i = 0; i < users; i++ )
    {
        if( strcmp( *(usernames+i), username ) == 0 )
        {
            break;
        }
    }
    //check if the given password matches the one that was loaded
    if( strcmp( *(passwords+i), password ) == 0 )
    {
        return LOGIN_SUCCESS;
    }
    
    //if this line is reached, login failed
    return LOGIN_FAILURE;
}

//TODO check and debug
int add_user( char *username, char *password )
{   
    //append the data to the file
    userlist = fopen( USERFILE, "a" );
    if( userlist == NULL )
    {
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