#include "src/client_thread.h"
#include "src/server_thread.h"
#include "src/string_functions.h"

#include <stdio.h>
#include <stdlib.h>

int main(int argc, char **argv){
    set_handler(sigint_handler, SIGINT);
    set_handler(sigusr1_handler, SIGUSR1);
    
    if(argc < 2){
        printf("Usage ./main.out port_number\n");
        return 0;
    }
    uint16_t port = (uint16_t)atoi(argv[1]);

    main_thread_func(port);

    return 0;
}
