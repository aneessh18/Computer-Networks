#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <string.h>
#include <sys/wait.h>
#include <fcntl.h>
#include <stdbool.h>
#include <poll.h>
struct frame
{
	int rpid,spid;
	char msg[100];
};
struct frame *pointer;
int main()
{
	mkfifo("token",0600);// creating the fifo
	int pid;
	scanf("%d",&pid);
	pointer=malloc(sizeof(struct frame));
	pointer->rpid=pid;
	pointer->spid=pid;
	strcpy(pointer->msg,"hi");
	printf("sending to %d \n",pointer->rpid);
	int fd=open("token",O_RDWR);
	write(fd,pointer,sizeof(struct frame));
	kill(pid,SIGUSR1);
}
