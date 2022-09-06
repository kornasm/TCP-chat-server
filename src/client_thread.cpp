#include "client_thread.h"

#include "server_thread.h"

#include <csignal>
#include <sys/socket.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <stdio.h>

extern volatile sig_atomic_t doWork;
extern volatile sig_atomic_t lastSignal;

void wait_for_signal(int signo){
    sigset_t mask; int x = 0;
    sigemptyset(&mask);
    sigaddset(&mask, signo);
    sigwait(&mask, &x);
}

void block_signal(int sigNo){
    sigset_t mask;
    sigemptyset(&mask);
    sigaddset(&mask, sigNo);
    pthread_sigmask(SIG_BLOCK, &mask, NULL);
}

void set_handler(void (*f)(int), int sigNo) {
    struct sigaction act;
    memset(&act, 0, sizeof(struct sigaction));
    act.sa_handler = f;

    if (sigaction(sigNo, &act, NULL) == -1)
        exit(EXIT_FAILURE);
}

void thread_sigusr2_handler(int signal){}

void thread_handle_messege_received(Client *client, char *buf){
    // log info on screen
    printf("\nMSG received from %d,  msg_length: %ld\n",client->socket, client->msg_size);
    printf("%s", client->last_msg);

    clear_telnet_msg(buf, client->msg_size);
    set_client_name(client, buf);
    if(check_if_blocked(buf))
        return; 
    prepare_messege_for_client(client, buf);
}

void notify_main_thread(Client *client){
    pthread_mutex_lock(client->server->msg_rec_mutex);
    client->server->last_client = client;
    kill(getpid(), SIGUSR1);
    wait_for_signal(SIGUSR2);
    pthread_mutex_unlock(client->server->msg_rec_mutex);
}

void *client_thread_work(void *args){
    block_signal(SIGINT);
    block_signal(SIGUSR1);
    set_handler(thread_sigusr2_handler, SIGUSR2);
    Client *client = (Client*)args;
    client->thread_id = pthread_self();
    char buf[MAX_MSG_SIZE + 1];

    while(doWork){
        client->msg_size = recv(client->socket, buf, MAX_MSG_SIZE, 0);
        
        if(client->msg_size == -1 && EINTR == errno){
            if(doWork){
                printf("MSG sending to:  %d\n", client->socket);
                printf("%s", client->server->last_client->last_msg);
                write(client->socket, client->server->last_client->last_msg, client->server->last_client->msg_size);
            }
            continue;
        }
        if(client->msg_size == 0){
            close(client->socket);
            client->socket = CLIENT_DISCONNECTED;
            notify_main_thread(client);
            break;
        }
        if(client->msg_size > 0){
            thread_handle_messege_received(client, buf);
            notify_main_thread(client);
        }
    }
    delete client;
    return NULL;
}