#include <errno.h>
#include <string.h>
#include <sys/un.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>

#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>


int sendfd(int socket, int fd) {
    char dummy = '$';
    struct msghdr msg;
    struct iovec iov;

    char cmsgbuf[CMSG_SPACE(sizeof(int))];

    iov.iov_base = &dummy;
    iov.iov_len = sizeof(dummy);

    msg.msg_name = NULL;
    msg.msg_namelen = 0;
    msg.msg_iov = &iov;
    msg.msg_iovlen = 1;
    msg.msg_flags = 0;
    msg.msg_control = cmsgbuf;
    msg.msg_controllen = CMSG_LEN(sizeof(int));

    struct cmsghdr* cmsg = CMSG_FIRSTHDR(&msg);
    cmsg->cmsg_level = SOL_SOCKET;
    cmsg->cmsg_type = SCM_RIGHTS;
    cmsg->cmsg_len = CMSG_LEN(sizeof(int));

    *(int*) CMSG_DATA(cmsg) = fd;
	printf("send fd is %d\n",fd);
    int ret = sendmsg(socket, &msg, 0);
	
    if (ret == -1) {
        puts("sendmsg failed with");
    }

    return ret;
}


int recvfd(int socket) {
    int len;
    int fd;
    char buf[1];
    struct iovec iov;
    struct msghdr msg;
    struct cmsghdr *cmsg;
    char cms[CMSG_SPACE(sizeof(int))];

    iov.iov_base = buf;
    iov.iov_len = sizeof(buf);

    msg.msg_name = 0;
    msg.msg_namelen = 0;
    msg.msg_iov = &iov;
    msg.msg_iovlen = 1;
    msg.msg_flags = 0;
    msg.msg_control = (caddr_t) cms;
    msg.msg_controllen = sizeof cms;

    len = recvmsg(socket, &msg, 0);

    if (len < 0) {
        puts("recvmsg failed with");
        return -1;
    }

    if (len == 0) {
        puts("recvmsg failed no data");
        return -1;
    }
	perror("recv msg");
    cmsg = CMSG_FIRSTHDR(&msg);
    memmove(&fd, CMSG_DATA(cmsg), sizeof(int));
    perror("recv msg");
    return fd;
}


int main()
{
	int usfd = socket(AF_UNIX,SOCK_STREAM,0);
	struct sockaddr_un server,client;
	server.sun_family = AF_LOCAL;
	strcpy(server.sun_path,"/home/indavarapu/Desktop/computer_networks/aneesh");
	unlink(server.sun_path);
	if(bind(usfd,(struct sockaddr *) & server,sizeof(server))<0)
	{
		puts("bind failed");
	}
	perror("bind");
	listen(usfd,10);
	perror("listen");
	int nsfd,len = sizeof(client);
	nsfd = accept(usfd,(struct sockaddr*) & client,&len);
	perror("accept");
	printf("%d\n",nsfd);
	if(nsfd<0)
	{
		puts("accept failed");
	}
	// sendfd close recvfd 
	int fd;
	mode_t mode = S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH;
	char filename[] = "input.txt";
	fd = open(filename,O_RDWR,mode);
	printf("%d %d %d \n",O_RDONLY,O_WRONLY,O_RDWR);
	printf("before writing %d\n",fd);
	char msg[] = "this is process 1\n";
	write(fd,msg,strlen(msg));
	sendfd(nsfd,fd);
	//close(fd);
	fd = recvfd(nsfd);
	printf("after 2 writes %d\n",fd);
	write(fd,msg,strlen(msg));
	int z=lseek(fd,0,SEEK_SET);
	printf("%d\n",z);
	char buf[100];
	printf("%d\n",fd);
	z=read(fd,buf,100);
	perror("read");
	printf("%d\n",z);
	printf("%s\n",buf);
	close(fd);
	
	return 0;
}
