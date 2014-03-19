#include "includes.h"

void *thread_function(void *ptr) {
    struct pair *data = (struct pair *) ptr;
    char buf[BUFSIZE];
    int p = 0;

    puts("Thread begin");

    printf("%d %d\n", data->from, data->to);

    while((p = read(data->from, buf, BUFSIZE*sizeof(char))) > 0) {
        write(data->to,buf, p);
    }

    p = 0;
    pthread_exit(&p);
}
