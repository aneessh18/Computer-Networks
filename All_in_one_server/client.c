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
struct client* pointer;
void handler(int signo)// it's time to communicate with echoserver
{
     // creating a named pipe to communicate with echo server
    char path[100];
    sprintf(path,"e%d.pipe",pointer->pid);
    mkfifo(path,0600);
    int ewfd=open(path,O_RDWR);
    char msg[100];
    pointer->state=2; // the process is with echo
    // sem_t *s1=sem_open(SNAME1,O_CREAT,0644,0); // for synchronizing
    // sem_t *s2=sem_open(SNAME2,O_CREAT,0644,0); 
    sem_t *s1=sem_open(SNAME1,0); // opening existing the semaphores in 
    sem_t *s2=sem_open(SNAME2,0);
    printf("this client is in the echo %d \n",pointer->pid);
    int c=0;
    while(c<3)
    {
        c++;
        int sz=read(0,msg,100);  // write to the echo server 
        msg[sz]='\0';
        strcpy(pointer->msg,msg);
        write(ewfd,pointer,sizeof(struct client));
        sem_post(s1);
        sem_wait(s2); // wait for echo server to write 
        sz=read(ewfd,pointer,sizeof(struct client));
        char extra[1000];
        printf("echo from echoserver  :  \n");
        write(1,pointer->msg,strlen(pointer->msg));
    }
    printf("client in normal server %d \n",pointer->pid);
    pointer->state=1;

}
int main()
{
    printf("client id %d \n",getpid());
    pid_t pid=getpid();
    char path[100];
    sprintf(path,"%d.pipe",pid); // client pipes 
    mkfifo(path,0600); // creating client pipe 

    struct pollfd pfds[2];
    int rfd=open(path,O_RDWR);
    pfds[0].fd=rfd; // one poll pipe to client pipe
    pfds[1].fd=0; // one poll pipe to STDIN
    pfds[0].events=POLLIN;
    pfds[1].events=POLLIN;
    pointer= malloc(sizeof (struct client));
    signal(SIGUSR2,handler);
     // creating a client structure
    pointer->pid=pid;
    pointer->state=0;

    int wfd=open("famousfifo",O_WRONLY);
    write(wfd,pointer,sizeof(struct client)); // for registering

    while(1)
    {
        int r=poll(pfds,2,10000);
        if(r>0)
        {
            if(pfds[0].revents!=0) // received input from client pipe
            {
                char msg[100];
                int sz=read(rfd,msg,100);
                char extra[1000];
                sprintf(extra,"message from server in group  : \n");
                write(1,extra,strlen(extra));
                write(1,msg,sz);
            }
            else if(pfds[1].revents!=0)
            {
                char msg[100];// writing in to famouse fifo
                int sz=read(0,msg,100);
                msg[sz]='\0';
                pointer->state=1;
                printf("client no : %d state : %d\n",pointer->pid,pointer->state);
                strcpy(pointer->msg,msg);
                write(wfd,pointer,sizeof(struct client)); // sending the message through client pointer
            }
        }
        else
        {
            printf("waiting\n");
        }
    }
}