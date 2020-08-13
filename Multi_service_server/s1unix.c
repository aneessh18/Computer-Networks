#include <sys/socket.h>
#include <sys/un.h>
#include <stdio.h>
#include <string.h>
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
    char *msg;
    msg="this is s1 u are assigned to me";
    int usfd;
	usfd = socket(AF_UNIX,SOCK_STREAM,0);
	struct sockaddr_un server;
	server.sun_family = AF_LOCAL;
	strcpy(server.sun_path,"/home/indavarapu/Desktop/computer_networks/Multi_service_server/aneesh");
	if(connect(usfd,(struct sockaddr *) & server,sizeof(server))<0)
	{
		puts("connect failed");
	}
	// recvfd sendfd close
    int fd = recvfd(usfd);
    send(fd, msg, strlen(msg),0);
    char buff[100];
    while(1)
    {
        int sz=recv(fd,buff,100,0);
        strcat(buff,"hi dude");
        send(fd, buff, strlen(buff), 0);
    }
}
