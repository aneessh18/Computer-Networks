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
    int fd=open("screen",O_RDWR);
    char msg[100];
    while(1)
    {
        int sz=read(fd,msg,100);
        if(sz<=0)
        continue;
        printf("----------------Today's News--------------\n ");
        printf("\n");
        write(1,msg,sz);
        printf("\n");
        printf("\n");
        printf("-----------------------------------------\n");
    }
}