#include <stdio.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <sys/stat.h>
#include <unistd.h>
#include <sys/ipc.h>
#include <semaphore.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <stdbool.h>
#include <sys/shm.h>
#include <signal.h>
#include <sys/types.h>

int main()
{
    int pid=getpid();
    printf("the live telecast id is %d \n",pid);
    char msg[100];
    char path[100];
    sprintf(path,"l%d.pipe",pid);
    mkfifo(path,0600);
    int fd=open(path,O_RDWR);
    while(1)
    {
        int sz=read(0,msg,100);
        msg[sz-1]='\0';
        if(strcmp(msg,"end")==0)
        break;
        write(fd,msg,sz);
    }
}