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

sem_t lock;
pthread_t reading,writing;

void *readingthread(void *args)
{
    sem_wait(&lock);
    pid_t pid=getpid();
    char path[100];
    sprintf(path,"%d.pipe",pid);
    mkfifo(path,0600); // creating client pipe 
    int fd;
    int c=0;
    while(1) // reading messages from client pipe
    {
        char msg[100];
        fd=open(path,O_RDONLY);
        int sz=read(fd,msg,100);
        if(sz==0 || sz==-1)
        {
            printf("waiting for msg \n");
            continue;
        }
        close(fd);
        c++;
        printf("message from server : %s \n",msg);
    }
}
struct client{
     pid_t pid;
    char msg[100];
    bool firsttime;
};
void *writingthread(void *args) 
{
    pid_t pid=getpid();
    struct client* pointer;
    pointer= malloc(sizeof(struct client)); // creating a client structure
    pointer->pid=pid;
    pointer->firsttime=true;
    char *myfifo="myfifo.pipe";
    mkfifo("myfifo.pipe",0600);
    int fd=open("myfifo.pipe",O_WRONLY);
    write(fd,pointer,sizeof(struct client));// first sending the details of the client
    close(fd);
    sem_post(&lock);
    int c=0;
    while(c<3) // writing  to famous fifo
    {
        char msg[100];
        int sz=read(0,msg,100); // taking the input 
        fd=open("myfifo.pipe",O_WRONLY);
        pointer->firsttime=false;
        strcpy(pointer->msg,msg);
        printf("my msg is %s \n",pointer->msg);
        write(fd,pointer,sizeof(struct client)); // sending the message through client pointer
        close(fd);
        c++;
    }
}
int main()
{
    printf("client id %d \n",getpid());
    sem_init(&lock,0,0);
    pthread_create(&reading,NULL,(void*) readingthread,NULL);
    pthread_create(&writing,NULL,(void*) writingthread,NULL);
    pthread_join(reading,NULL);
    pthread_join(writing,NULL);
}