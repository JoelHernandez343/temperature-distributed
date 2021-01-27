/**
 * Joel Hernández @ 2020
 * Github: https://github.com/JoelHernandez343
 * 
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/types.h> 
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>

#include "defs.h"

#define LOG "[Client] "

char *hostname;
int port;

void receive_data(int client_socket){
    printf(LOG"Receiving data.\n");

    int received = 0, length = sizeof(Clock);

    Clock clock;
    memset(&clock, 0, sizeof(Clock));

    while (received < length){
        received += read(client_socket, &clock + received, length - received);
    }

    printf("Seconds: %d\n", clock.seconds % 60);
    printf("Minutes: %d\n", clock.minute);
    printf("Hours: %d\n", clock.hour);
    printf("Temp: %f\n", clock.temp);

    printf(LOG"Data received, closing connection.\n");
}

int hostname_to_ip(char * hostname , char* ip){
    struct hostent *he;
    struct in_addr **addr_list;
    int i;

    if ( (he = gethostbyname( hostname ) ) == NULL){
        // get the host info
        herror("gethostbyname");
        return 1;
    }

    addr_list = (struct in_addr **) he->h_addr_list;

    for(i = 0; addr_list[i] != NULL; i++){
        //Return the first one;
        strcpy(ip , inet_ntoa(*addr_list[i]) );
        return 0;
    }

    return 1;
}


void initialize_client(int *client_socket){
    struct sockaddr_in server_address;
    
    memset(&server_address, 0, sizeof(server_address));
    server_address.sin_family = AF_INET;
    server_address.sin_port = htons(port);

    // Solo se usa si se conecta de forma remota
    char ip[100];
    hostname_to_ip(hostname, ip);

    if (inet_pton(AF_INET, ip, &server_address.sin_addr) <= 0){
        printf(ERR_LOG"IP %s could not be set.\n", ip);
        exit(EXIT_FAILURE);
    }

    printf(LOG"Creating socket.\n");
    if ((*client_socket = socket(AF_INET, SOCK_STREAM, 0)) < 0){
        printf(ERR_LOG"Couldn't create socket.\n");
        exit(EXIT_FAILURE);
    }

    printf(LOG"Connecting to %s:%d.\n", ip, port);
    if (connect(*client_socket, (struct sockaddr *)&server_address, sizeof(server_address))){
        printf(ERR_LOG"Unable connect to server.\n");
        exit(EXIT_FAILURE);
    }
}

int main(int argc, char * argv[]){

    if (argc != 3){
        printf("Its needed two parameters %d: <ip> <port>", argc);
        exit(EXIT_FAILURE);
    }

    hostname = argv[1];
    port = atoi(argv[2]);

    int client_socket;
    initialize_client(&client_socket);

    receive_data(client_socket);
    
    close(client_socket);
}