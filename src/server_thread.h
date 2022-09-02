#ifndef SERVER_THREAD_H
#define SERVER_THREAD_H

#include <pthread.h>
#include <stdint.h>
#include <vector>

struct Client;

struct ChatData{
    pthread_t last_msg_t;
    Client *last_client;
    pthread_mutex_t *msg_rec_mutex;
};

ChatData *init_chat_data();

void sigint_handler(int signal);
void sigusr1_handler(int signal);

int create_socket(uint16_t port);
int accept_new_client(int server_socket);

Client *init_new_client(ChatData *chat, int client_socket);
void create_client_thread(Client *client, std::vector<pthread_t> *threads);

void server_handle_client_disconnected(Client * client, std::vector<pthread_t> *threads);
void server_handle_messege(Client *client, std::vector<pthread_t> *threads);
void main_thread_func(uint16_t port);

#endif