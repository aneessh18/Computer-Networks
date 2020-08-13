#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <string.h>
#include <sys/wait.h>
#define READ_END 0
#define WRITE_END 1

int main( int argc ,char * argv[])  // getting the programs
{
    FILE* f = popen("./test","r");
    dup2(fileno(f),0);
    f= popen("./test","r");
    int d;
    fscanf(f,"%d",&d);
    printf("%d\n",d);
}