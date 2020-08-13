#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <string.h>
#include <sys/wait.h>
#define READ_END 0
#define WRITE_END 1
// communication with echo server 2
int main()
{
    FILE *fp;
    int  pid,x;
    int fd[2];
    int fd1=dup(1);
    pipe(fd);
    dup2(fd[WRITE_END],1);
    fp=popen("./test","w");
    while(1)
    {
        char buf[100];
        int sz=read(0,buf,100);
        write(fileno(fp),buf,sz);
        sz=read(fd[0],buf,100);
        write(1,buf,sz);
    }
}