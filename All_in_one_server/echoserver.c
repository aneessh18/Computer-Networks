#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <string.h>
#include <sys/wait.h>
#include <fcntl.h>
#include <stdbool.h>
#include <poll.h>
#include <semaphore.h>
#define SNAME1 "/mysem1"
#define SNAME2 "/mysem2"
struct client{
    pid_t pid;
    char msg[100];
    int state;
};
int main()
{
    sem_t *s1=sem_open(SNAME1,0); // opening existing the semaphores in 
    sem_t *s2=sem_open(SNAME2,0);
    int fd=open("echopipe",O_RDWR);
    struct client* pointer;
    pointer= malloc(sizeof (struct client));
    while (1)
    {
        char msg[100];
        sem_wait(s1);
        int sz=read(fd,pointer,sizeof(struct client));
        char extra[1000];
        sprintf(extra,"message from CLIENT : %d ---> %s",pointer->pid,pointer->msg);
        write(1,extra,strlen(extra));
        fflush(stdout);
        write(fd,pointer,sizeof(struct client)); // after writing to client release the semaphore
        sem_post(s2);
    }
}