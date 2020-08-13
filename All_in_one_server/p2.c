#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/shm.h>
#include <string.h>
#include  <fcntl.h>
#include <sys/wait.h>

int main()
{
    char *p2fifo="p2fifo";
    mkfifo(p2fifo,0600); // creating a fifo for communicating

    int wfd=open(p2fifo,O_WRONLY); // writing in to the fifo
    while(1)
    {
        char msg[100];
        printf("typing... \n");
        int sz=read(0,msg,100);
        write(wfd,msg,sz);
    }
}