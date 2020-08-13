#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/types.h>
#include <unistd.h>
#include <semaphore.h>
#include <stdbool.h>
#define SNAME1 "/mysem1"
#define SNAME2 "/mysem2"
// mode of communication of pid's is through shared memory
struct memory{
    int num;
    int pid1,pid2;
    bool isp1ready;
    bool isp2ready;
};

struct memory* x; // for sharing x and y

void handler(int signum)
{
    if(signum==SIGUSR2)
    {
        printf("p1 has raised a signal\n");
    }
}

int main()
{
    int pid=getpid();// current process id 
    int shmid;
    int key=12345;
    //creating the shared memory
    shmid=shmget(key,sizeof(struct memory),IPC_CREAT | 0666);
    sem_t *s1=sem_open(SNAME1,0); // opening existing the semaphores in 
    sem_t *s2=sem_open(SNAME2,0); 
    x=(struct memory*)shmat(shmid,NULL,0);
    x->pid2=pid;
    x->isp2ready=true;
    signal(SIGUSR2,handler);// assigning the signal handler function
    int c=0;
    x->isp2ready=false;
    while(!x->isp1ready);
    x->isp2ready=false;
    while(c<3)
    {
       // sem_wait(s1);
        while(!x->isp1ready); // wait untill p1 is ready
        x->isp2ready=false;
        printf("p2 read the value of x \n");
        printf("value of x is %d\n",x->num);
        printf("p2 has written the value the value of y \n");
        printf("value of y is %d\n",x->num);
        x->num*=2;
        x->isp2ready=true;
        kill(x->pid1,SIGUSR1);
        c++;
    }
    shmdt((void*)x);
}

