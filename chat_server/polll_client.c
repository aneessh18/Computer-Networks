#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <string.h>
#include <sys/wait.h>
#include <fcntl.h>
#include <stdbool.h>
#include <semaphore.h>
#include <pthread.h>
#include <poll.h>
sem_t lock;

struct client{
     pid_t pid;
    char msg[100];
    bool firsttime;
};

int main()
{
    printf("client id %d \n",getpid());
    sem_init(&lock,0,0);

    struct pollfd thread[2];
    pid_t pid=getpid();
    char path[100];
    sprintf(path,"%d.pipe",pid);
    mkfifo(path,0600); // creating client pipe 

    
    int rfd=open(path,O_RDWR);
    thread[0].fd=rfd; // one poll pipe to client pipe
    thread[1].fd=0; // one poll pipe to STDIN
    thread[0].events=POLLIN;
    thread[1].events=POLLIN;
    struct client* pointer;
    pointer= malloc(sizeof(struct client)); // creating a client structure
    pointer->pid=pid;
    pointer->firsttime=true;
    int fd=open("myfifo.pipe",O_WRONLY);
    write(fd,pointer,sizeof(struct client));// first sending the details of the client
    while(1)
    {
        int r=poll(thread,2,5000);
        if(r>0)
        {
            if(thread[0].revents!=0) // received input from client pipe
            {
                char msg[100];
                read(thread[0].fd,msg,100);
                printf("message from server : %s \n",msg);
            }
            else if(thread[1].revents!=0)
            {
                char msg[100];
                int sz=read(thread[1].fd,msg,100); // taking the input 
                pointer->firsttime=false;
                strcpy(pointer->msg,msg);
                printf("my msg is %s \n",pointer->msg);
                write(fd,pointer,sizeof(struct client)); // sending the message through client pointer
            }
        }
        else
        {
            printf("waiting\n");
        }
        
    }
}