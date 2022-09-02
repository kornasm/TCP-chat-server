#include "server_thread.h"

#include "client_thread.h"

#include <sys/socket.h>
#include <netinet/in.h>

#include <stdlib.h>
#include <string.h>
#include <errno.h>

#include <vector>
#include <algorithm>

// variable that store info whether program should exit, set in SIGINT handler
volatile sig_atomic_t doWork = 1; 
volatile sig_atomic_t lastSignal = 0;

void sigint_handler(int signal){ // received only by main thread
    doWork = 0;
    lastSignal = signal;
    block_signal(SIGUSR1); // main thread shouldn't receive any information after sigint
}

void sigusr1_handler(int signal){
    lastSignal = signal;
}

ChatData *init_chat_data(){
    ChatData *chat = new ChatData();
    chat->last_msg_t = 0;
    chat->msg_rec_mutex = new pthread_mutex_t();
    return chat;
}

int create_socket(uint16_t port){
    int sock = socket(PF_INET, SOCK_STREAM, 0);
    int t = 1;
    struct sockaddr_in addr;
    memset(&addr, 0, sizeof(struct sockaddr_in));
    addr.sin_family = AF_INET;
    addr.sin_port = htons(port);
    addr.sin_addr.s_addr = htonl(INADDR_ANY);
    setsockopt(sock, SOL_SOCKET, SO_REUSEADDR, &t, sizeof(t));
    if(bind(sock, (const sockaddr *)(&addr), sizeof(addr)) == -1){
        exit(EXIT_FAILURE);
    }
    listen(sock, 5);
    return sock;
}

int accept_new_client(int server_socket){
    int client_sock;
    client_sock = accept(server_socket, NULL, NULL);
    return client_sock;
}

void server_handle_client_disconnected(Client * client, std::vector<pthread_t> *threads){
    pthread_t finishing_thread = client->thread_id;
    threads->erase(std::remove(threads->begin(), threads->end(), finishing_thread), threads->end());
    pthread_kill(client->thread_id, SIGUSR2);
    pthread_join(finishing_thread, NULL);
}
void server_handle_messege(Client *client, std::vector<pthread_t> *threads){
    pthread_t sending_thread = client->thread_id;
    //lastSignal = SIGUSR2;
    for(unsigned int i = 0; i < threads->size(); i++){
        if((*threads)[i] != sending_thread){
            pthread_kill((*threads)[i], SIGUSR2);
        }
    }
    pthread_kill(sending_thread, SIGUSR2);
    //lastSignal = 0;
}

inline bool check_client_disconnected(Client *client){
    return client->socket == 0;
}

Client *init_new_client(ChatData *chat, int client_socket){
    Client *new_client_data = new Client();
    new_client_data->server = chat;
    new_client_data->socket = client_socket;
    return new_client_data;
}

void create_client_thread(Client *client, std::vector<pthread_t> *threads){
    pthread_t th;
    pthread_create(&th, NULL, client_thread_work, (void*)client);
    threads->push_back(th);
}

void main_thread_func(uint16_t port){
    std::vector<pthread_t> threads;
    int server_socket = create_socket(port);

    ChatData *chat = init_chat_data();
    while(doWork){
        int new_client_socket = accept_new_client(server_socket);
        if(new_client_socket == -1 && EINTR == errno){ // signal received
            if(lastSignal == SIGINT)
                break;

            Client *client = chat->last_client;
            if(check_client_disconnected(client)){
                server_handle_client_disconnected(client, &threads);
            }
            else{
                server_handle_messege(client, &threads);
            }
        }
        if(new_client_socket >= 0){
            Client *client = init_new_client(chat, new_client_socket);
            create_client_thread(client, &threads);
        }
    }

    pthread_mutex_lock(chat->msg_rec_mutex);
    for(unsigned int i = 0; i < threads.size(); i++){
        pthread_kill(threads[i], SIGUSR2);
        pthread_join(threads[i], NULL);
    }
    pthread_mutex_unlock(chat->msg_rec_mutex);

    delete chat->msg_rec_mutex;
    delete chat;

    return;
}