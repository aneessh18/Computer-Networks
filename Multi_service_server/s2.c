#include <sys/socket.h>
#include <stdio.h>
#include <string.h>
int main()
{
    char *msg;
    msg="this is s2 u are assigned to me";
    send(1,msg,strlen(msg),0);
    char buff[100];
    while(1)
    {
        int sz=recv(0,buff,100,0);
        strcat(buff,"hi dude");
        send(1,buff,strlen(buff),0);
    }
}