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

#define SERVER_IP "127.0.0.1"
#define SOCKET_PORT 9375
#define VERBOSE 1
#define DEBUG 1
#define BUFSIZE 256

struct pair {
    int from;
    int to;
};


#define MY_ERROR1(text) if (DEBUG) {                                    \
        int myerr = errno;                                              \
        fprintf(stderr, "%s:%d, *%s* %s '%s'[%d]\n",                    \
                __FILE__, __LINE__, __FUNCTION__, text, strerror(myerr), myerr); \
        fflush(stderr);                                                 \
    }

void *thread_function(void *ptr);
