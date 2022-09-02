#include "client_thread.h"

#include "server_thread.h"

#include <sys/socket.h>

#include <stdlib.h>
#include <string.h>
#include <errno.h>

#include <iostream>

extern volatile sig_atomic_t doWork;
extern volatile sig_atomic_t lastSignal;

void wait_for_signal(int signo){
    sigset_t mask; int x = 0;
    sigemptyset(&mask);
    sigaddset(&mask, signo);
    sigwait(&mask, &x);
}

void block_signal(int sigNo, int mode, bool unblockall){
    sigset_t mask;
    sigemptyset(&mask);
    if(unblockall){
        pthread_sigmask(SIG_SETMASK, &mask, NULL);
        return;
    }
    sigaddset(&mask, sigNo);
    pthread_sigmask(mode, &mask, NULL);
}

void set_handler(void (*f)(int), int sigNo) {
    struct sigaction act;
    memset(&act, 0, sizeof(struct sigaction));
    act.sa_handler = f;

    if (sigaction(sigNo, &act, NULL) == -1)
        exit(EXIT_FAILURE);
}

void thread_sigusr2_handler(int signal){}

void thread_handle_client_disconnected(Client *client){
    pthread_mutex_lock(client->server->msg_rec_mutex);
    close(client->socket);
    client->socket = 0;
    client->server->last_client = client;
    kill(getpid(), SIGUSR1);
    wait_for_signal(SIGUSR2);
    pthread_mutex_unlock(client->server->msg_rec_mutex);
}
void thread_handle_messege_received(Client *client, char *buf){
    clear_telnet_msg(buf, client->msg_size);
    set_client_name(client, buf);
    if(check_if_blocked(buf))
        return; 
    prepare_messege_for_client(client, buf);
    
    pthread_mutex_lock(client->server->msg_rec_mutex);
    client->server->last_msg_t = pthread_self();
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
        
        // if logujący na stdout
        if(client->msg_size > 0){
            std::cout << "\nMSG received from " << client->socket << ",  msg_length: " << client->msg_size << "\n";
            for(int i = 0; i < client->msg_size; i++){
                std::cout << buf[i];
            }
        }
        
        if(client->msg_size == -1){
            if(EINTR == errno){
                if(doWork){
                    std::cout << "MSG sending to:  " << client->socket << "\n";
                    for(int i = 0; i < client->server->last_client->msg_size; i++){
                        std::cout << client->server->last_client->last_msg[i];
                    }
                    write(client->socket, client->server->last_client->last_msg, client->server->last_client->msg_size);
                }
                continue;
            }
        }
        if(client->msg_size == 0){
            thread_handle_client_disconnected(client);
            break;
        }
        if(client->msg_size > 0){
            thread_handle_messege_received(client, buf);
        }
    }
    delete client;
    return NULL;
}