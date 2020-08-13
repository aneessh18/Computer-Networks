#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/shm.h>
#include <string.h>
#include  <fcntl.h>
#include <sys/wait.h>
#include <stdbool.h>

int main() // reading from server and displaying on stdout
{
    char msg[100];
    while(1)
    {
        int sz=read(0,msg,100);
        if(sz<=0)
        continue;
        msg[sz]='\0';
        printf("this is process5 \n");
        write(1,msg,sz);
    }
    
}