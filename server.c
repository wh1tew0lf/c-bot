#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>
#include <pthread.h>
#include <error.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netinet/tcp.h>
#include <arpa/inet.h>
#include <errno.h>
#include <string.h>
#include "includes.h"

int main(int argc, char *argv[]) {


    int port = (argc > 1) ? atoi(argv[1]) : SOCKET_PORT;
    printf("PORT: %d\n", port);

    
    int sockfd = socket(AF_INET, SOCK_STREAM, 0);
    struct sockaddr_in server_address;
    server_address.sin_family = AF_INET;
    server_address.sin_addr.s_addr = htonl(INADDR_ANY);
    server_address.sin_port = htons(port);

    int skip = 0; //if error happen _skip_ other steps before exit
 
    if (!skip && bind(sockfd, (struct sockaddr *) &server_address, sizeof(server_address))) {
        MY_ERROR1("Bind");
        skip = 1;
    } else if (VERBOSE) {
        printf("bind is ok\n");
    }

    if (!skip && listen(sockfd, 5)) {
        MY_ERROR1("Listen");
        skip = 2;
    } else if (VERBOSE) {
        printf("listen is ok\n");
    }


        
    struct sockaddr_in client_address;
    int client_len = sizeof(client_address);
    int client_sockfd = accept(sockfd, (struct sockaddr *) &client_address, &client_len);
    printf("connected!\n");

    struct pair uin;
    struct pair uout;
    struct pair uerr;

    uin.from = STDIN_FILENO;
    uin.to = client_sockfd;
    dup2(STDIN_FILENO, uin.from);
    dup2(client_sockfd, uin.to);
        
    uout.from = client_sockfd;
    uout.to = STDOUT_FILENO;
    dup2(client_sockfd, uout.from);
    dup2(STDOUT_FILENO, uout.to);

    uerr.from = client_sockfd;
    uerr.to = STDERR_FILENO;
    dup2(client_sockfd, uerr.from);
    dup2(STDERR_FILENO, uerr.to);

    pthread_t tin,tout,terr;

    pthread_create(&tin, NULL, thread_function, (void*) &uin);
    pthread_create(&tout, NULL, thread_function, (void*) &uout);
    pthread_create(&terr, NULL, thread_function, (void*) &uerr);

    pthread_join(tin, NULL);
    pthread_join(tout, NULL);
    pthread_join(terr, NULL);
        
    close(client_sockfd);
    

    if (close(sockfd)) {
        MY_ERROR1("Close socket");
    }

    fprintf(stdout, "Server stop\n");
    return EXIT_SUCCESS;
}


