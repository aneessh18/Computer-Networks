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

struct client{
    pid_t pid;
    int gid;
    char msg[100];
    bool firsttime;
};

int main()
{
    printf("client id %d \n",getpid());
    struct pollfd thread[2];
    pid_t pid=getpid();
    char path[100];
    sprintf(path,"%d.pipe",pid); // client pipes 
    mkfifo(path,0600); // creating client pipe 

    
    int rfd=open(path,O_RDWR);
    thread[0].fd=rfd; // one poll pipe to client pipe
    thread[1].fd=0; // one poll pipe to STDIN
    thread[0].events=POLLIN;
    thread[1].events=POLLIN;


    struct client* pointer;
    pointer= malloc(sizeof (struct client)); // creating a client structure
    printf("which group you wanna be ?\n");
    scanf("%d",&pointer->gid);
    pointer->pid=pid;
    pointer->firsttime=true;


    char gpipe[100];   
    sprintf(gpipe,"%d.pipe",pointer->gid);
    int fd=open(gpipe,O_WRONLY); // opening a group pipe
    write(fd,pointer,sizeof(struct client));// first sending the details of the client



    while(1)
    {
        int r=poll(thread,2,10000);
        if(r>0)
        {
            if(thread[0].revents!=0) // received input from client pipe
            {
                char msg[100];
                int sz=read(rfd,msg,100);
                char extra[1000];
                sprintf(extra,"message from server in group %d : ",pointer->gid);
                write(1,extra,strlen(extra));
                write(1,msg,sz);
            }
            else if(thread[1].revents!=0)
            {
                char msg[100];
                fflush(stdin);
                printf("2\n");
                int sz=read(0,msg,100);
                msg[sz]='\0'; // taking the input from group pipe
                write(1,msg,sz);
                pointer->firsttime=false;
                printf("1\n");
                strcpy(pointer->msg,msg);
                printf("2\n");
                write(fd,pointer,sizeof(struct client)); // sending the message through client pointer
                printf("2\n");
            }
        }
        else
        {
            printf("waiting\n");
        }
        
    }
}