#include <stdio.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <unistd.h>
struct mesg_buff{
    long mesg_type;
    char mesg_text[100];
    //pid_t pid;
}msg;


int main() // this is the receiver's process for writing in to message queue
{
    
    key_t key;
    int msgid;
    key=ftok("msgqueue",65);
    msgid=msgget(key,0666 | IPC_CREAT);
    msg.mesg_type=1;
    //msg.pid=getpid();
    while(1)
    {

        printf("write data :\n");
        int sz=read(0,msg.mesg_text,100);
        msg.mesg_text[sz-1]='\0';
        msgsnd(msgid,&msg,sizeof(msg),0);
    }
    return 0;
}