#ifndef MY_STRINGS_FUNCTIONS_H
#define MY_STRINGS_FUNCTIONS_H

#define MAX_MSG_SIZE 100
#define MAX_NAME_SIZE 15

class Client;

char *find_name(char *msg);
int get_name_length(char *msg);
bool check_if_blocked(char *msg);

void clear_telnet_msg(char* msg, long int size);

void set_client_name(Client *client, char *msg);
void prepare_messege_for_client(Client *client, char *msg);

#endif