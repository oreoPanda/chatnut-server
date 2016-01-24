/*server.h*/

#define USERFILE "users.txt"

int add_user(char *username, char *password);
int find_user(char *username, char *password);
int load_all_users(void);