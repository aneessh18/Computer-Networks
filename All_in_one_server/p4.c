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
struct memory
{
    int pid1,pid2;
    bool isp1ready;
    bool isp2ready;
};

int main() // sending signal to server
{
    int pid=getpid();
    int shmid;
    int key=12345;
    shmid=shmget(key,sizeof(struct memory),IPC_CREAT | 0666);
    struct memory *x;
    x=(struct memory*)shmat(shmid,NULL,0);
    x->pid1=pid;
    x->isp1ready=true;
    printf("%d\n",x->pid2);
    while(!x->isp2ready);

    int c;
    do
    {
        scanf("%d",&c);
        if(c!=-1)
        kill(x->pid2,SIGUSR1);
    } while (c!=-1);
    
    shmdt((void*)x);
    shmctl(shmid, IPC_RMID, NULL);

}