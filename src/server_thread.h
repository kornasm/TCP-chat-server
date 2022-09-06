#ifndef SERVER_THREAD_H
#define SERVER_THREAD_H

// -1 represents dissconected client as it is invalid socket number
#define CLIENT_DISCONNECTED -1

#include <pthread.h>
#include <stdint.h>
#include <vector>

struct Client;

struct ChatData{
    Client *last_client;
    pthread_mutex_t *msg_rec_mutex;
};

void sigint_handler(int signal);
void sigusr1_handler(int signal);

int create_socket(uint16_t port); // 

ChatData *init_chat_data();
void add_new_client(ChatData *chat, int socket, std::vector<pthread_t> *threads);

inline bool check_client_disconnected(Client *client);
void server_handle_client_disconnected(Client * client, std::vector<pthread_t> *threads);
void server_handle_messege(Client *client, std::vector<pthread_t> *threads);
void main_thread_func(uint16_t port);

#endif