#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>

int main()
{
    char buf[100];
    read(0,buf,100);
    write(1,buf,100);
    return 0;
}