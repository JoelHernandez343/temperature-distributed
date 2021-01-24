/**
 * Joel Hernández @ 2020
 * Github: https://github.com/JoelHernandez343
 * 
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <signal.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>

#include <pthread.h>

#include <syslog.h>

#include "defs.h"
#include "thread.h"
#include "daemon.h"
#include "sensor.h"

#define LOG "[Server] "
#define CLIENTS 5

Clock data;

pthread_mutex_t lock;
pthread_t thread;
int exit_flag = 1;

int server_socket;

void signal_handler(int sig){
    if (sig == SIGCHLD){
        int status;
        syslog(LOG_INFO, LOG"A child has finished.\n");
        wait(&status);
    }
    else if (sig == SIGINT){
        syslog(LOG_INFO, LOG"Termination requested.\n");
        syslog(LOG_INFO, LOG"Ending server process and freeing up resources.\n");
        
        close(server_socket);

        exit_flag = 0;
        syslog(LOG_INFO, LOG"Waiting for worker thread.\n");
        pthread_join(thread, NULL);
        pthread_mutex_destroy(&lock);
        
        syslog(LOG_INFO, LOG"Finished.\n");

        exit(EXIT_SUCCESS);
    }
}

void initialize_signals(){
    if (signal(SIGCHLD, signal_handler) == SIG_ERR){
        syslog(LOG_INFO, ERR_LOG"Error binding handler w/ SIGCHLD\n");
        exit(EXIT_FAILURE);
    }

    if (signal(SIGINT, signal_handler) == SIG_ERR){
        syslog(LOG_INFO, ERR_LOG"Error binding handler w/ SIGINT\n");
        exit(EXIT_FAILURE);
    }
}

void initialize_server(){
    struct sockaddr_in server_address;

    memset(&server_address, 0, sizeof(server_address));
    server_address.sin_family = AF_INET;
    server_address.sin_port = htons(PORT);
    server_address.sin_addr.s_addr = INADDR_ANY;

    syslog(LOG_INFO, LOG"Creating socket.\n");
    if ((server_socket = socket(AF_INET, SOCK_STREAM, 0)) < 0){
        syslog(LOG_INFO, ERR_LOG"Couldn't create socket.\n");
        exit(EXIT_FAILURE);
    }

    syslog(LOG_INFO, LOG"Configuring socket.\n");
    if (bind(server_socket, (struct sockaddr *)&(server_address), sizeof(server_address)) < 0){
        syslog(LOG_INFO, ERR_LOG"Socket couldn't be set.\n");
        exit(EXIT_FAILURE);
    }

    syslog(LOG_INFO, LOG"Setting client acceptance.\n");
    if (listen(server_socket, CLIENTS) < 0){
        syslog(LOG_INFO, ERR_LOG"Cliet queue couldn't be set.\n");
        exit(EXIT_FAILURE);
    }
}

void initialize_thread(){
    pthread_mutex_init(&lock, NULL);
    launch_thread();
}

void bind_data(char buffer[5 * 32], char *parameters[5]){
    memset(buffer, 0, 5 * 32);

    for (int i = 0; i < 5; ++i){
        parameters[i] = buffer + i * 32;
    }
}

void client_handler(int client_socket, Clock clock){
    syslog(LOG_INFO, "[CLIENT] Sending data to client.\n");

    print_clock(&clock);

    int sent = 0, length = sizeof(clock);
    while (sent < length){
        sent += write(client_socket, &clock + sent, length - sent);
    }

    close(client_socket);
    syslog(LOG_INFO, "[CLIENT] Data sent.\n");

    exit(EXIT_SUCCESS);
}

void server_process(){
    daemonize();

    initialize_signals();
    initialize_server();
    initialize_thread();

    int client_socket;

    while (1){
        syslog(LOG_INFO, LOG"Waiting for connections...\n");

        if ((client_socket = accept(server_socket, NULL, NULL)) < 0){
            syslog(LOG_INFO, ERR_LOG"Client couldn't be served\n");
            exit(EXIT_FAILURE);
        }

        syslog(LOG_INFO, "Cliente!!!!");

        Clock clock;
        blocking(GET_DATA, &clock);

        if (!fork()){
            client_handler(client_socket, clock);
        }
    }
}