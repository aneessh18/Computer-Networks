#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <string.h>
#include <sys/wait.h>
#include <fcntl.h>
#include <stdbool.h>
// this is server
struct client
{
    pid_t pid;
    char msg[100];
    bool firsttime;
};
pid_t p[100];
int main()
{
    char *myfifo="myfifo.pipe";
    mkfifo("myfifo.pipe",0600);  // creating famous fifo
    int rfd,wfd;
    rfd=open(myfifo,O_RDONLY);
    int no_of_clients=0,errno;
    struct client *pointer=malloc(sizeof(struct client));
    while(1)
    {
        int sz=read(rfd,pointer,1000) ; // reading from famous fifo (details of client)
        if(sz==-1)
        {
            printf("\n %s \n",strerror(errno));
            exit(0);
        }
        if(sz==0 ) // if client has not written any thing
        {
            continue;
        }
        printf("%d  \n",pointer->pid);
        if(pointer->firsttime)
        {
            printf("client id %d \n",pointer->pid);
            p[no_of_clients++]=pointer->pid;
        }
        else
        {
            for(int i=0;i<no_of_clients;i++)
            {
            if(p[i]!=pointer->pid)
            {
                char path[100];
                sprintf(path,"%d.pipe",p[i]);
                wfd=open(path,O_WRONLY);
                write(wfd,pointer->msg,strlen(pointer->msg));
                close(wfd);
            }
            }
        }
        printf("no of clients are %d \n",no_of_clients);
        
    }
    close(rfd);
}
