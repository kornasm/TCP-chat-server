#ifndef CLIENT_THREAD_H
#define CLIENT_THREAD_H

#include "string_functions.h"

#include <pthread.h>
#include <signal.h>
#include <unistd.h>

struct ChatData;

struct Client{
    pthread_t thread_id;
    int socket;

    ssize_t name_len = 0;
    char name[MAX_NAME_SIZE];
    ssize_t msg_size = 0;
    char last_msg[MAX_MSG_SIZE + MAX_NAME_SIZE + 1];
    
    ChatData *server;
};

void wait_for_signal(int signo);
void block_signal(int sigNo, int mode = SIG_BLOCK, bool unblockall = false);
void set_handler(void (*f)(int), int sigNo);
void thread_sigusr2_handler(int signal); // empty handler. The only reason for this is to interrupt
                                         // blocking recv without exiting thread


void thread_handle_client_disconnected(Client *client);
void thread_handle_messege_received(Client *client, char *buf);

void *client_thread_work(void *args);

#endif