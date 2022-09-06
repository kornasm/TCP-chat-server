#include "client_thread.h"
#include "string_functions.h"

#include <string.h>
#include <stdlib.h>

char *find_name(char *msg){
    char *result;
    if((result = strstr(msg, "imie ")) == msg){
        return result + 5; // 5 because of length of "imie" plus space
    }
    return NULL;
}

int get_name_length(char *msg){
    char *temp = msg;
    int result = 0;
    while(*temp != ' ' && *temp != '\0'){
        temp++;
        result++;
    }
    return result;
}

bool check_if_blocked(char *msg){
    if(strstr(msg, "blockme") != NULL){
        return true;
    }
    return false;
}

// The function assumes that buffer size is bigger (at least) by one byte than message size.
// If the assumption is not satisfied, SegFault will be thrown
void clear_telnet_msg(char* msg, long int size){
    for(int i = 0; i < size; i++){
        if(msg[i] < 32 && msg[i] != 10){
            msg[i] = ' ';
        }
    }
    msg[size] = '\0';
}
void set_client_name(Client *client, char *msg){
    char *name;
    if((name = find_name(msg)) != NULL){
        int len = get_name_length(name);
        if(len < MAX_NAME_SIZE - 1){
            client->name_len = len;
            strncpy(client->name, name, len);
            client->name[len] = '\0';
        }
        
        
    }
}
void prepare_messege_for_client(Client *client, char *msg){
    client->last_msg[0] = '\0';
    if(client->name_len > 0){
        strcat(client->last_msg, client->name);
        strcat(client->last_msg, ": ");
    }
    strcat(client->last_msg, msg);
    if(client->name_len > 0){
        client->msg_size += client->name_len + 2;
    }
}