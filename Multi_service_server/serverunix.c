#include <sys/socket.h>
#include <sys/un.h>
#include <stdio.h>
#include <string.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <poll.h>
// main server routing clients to s1 and s2
#define port1 8080
#define port2 8081
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
    int sfd1,sfd2;
    sfd1=socket(AF_INET,SOCK_STREAM,0);
    sfd2=socket(AF_INET,SOCK_STREAM,0);
    if(sfd1<0 || sfd2<0)
    {
        printf("socket failed \n");
        return -1;
    }
    // bind the sockets

    struct sockaddr_in server1,server2,client;
    server1.sin_family=AF_INET;
    server1.sin_port=htons(port1);
    server1.sin_addr.s_addr=INADDR_ANY;

    server2.sin_family=AF_INET;
    server2.sin_port=htons(port2);
    server2.sin_addr.s_addr=INADDR_ANY;

    if(bind(sfd1,(struct sockaddr*)&server1,sizeof(server1))<0)
    {
        printf("bind failed\n");
        return -1;
    }

    if(bind(sfd2,(struct sockaddr*)&server2,sizeof(server2))<0)
    {
        printf("bind failed\n");
        return -1;
    }
    listen(sfd1,3);
    listen(sfd2,3);
    struct pollfd pfd[2];
    pfd[0].fd=sfd1; // linked to server1
    pfd[1].fd=sfd2; // linked to server2
    pfd[0].events=POLLIN;
    pfd[1].events=POLLIN;
    int no_of_clients=0;
    int usfd = socket(AF_UNIX,SOCK_STREAM,0);
	struct sockaddr_un server,client1, client2;
	server.sun_family = AF_LOCAL;
	strcpy(server.sun_path,"/home/indavarapu/Desktop/computer_networks/Multi_service_server/aneesh");
	unlink(server.sun_path);
	if(bind(usfd,(struct sockaddr *) & server,sizeof(server))<0)
	{
		puts("bind failed");
	}
	listen(usfd,10);
	int nsfd1,nsfd2,len = sizeof(client1);
	nsfd1 = accept(usfd,(struct sockaddr*) & client1,&len);
	nsfd2 = accept(usfd,(struct sockaddr*) &client2,&len);
    while(1)
    {
        int r=poll(pfd,2,5000);
        if(r>0)
        {
            if(pfd[0].revents>0) // client request for server1
            {
                int nsfd,c=sizeof(server1);
                if((nsfd=accept(pfd[0].fd,(struct sockaddr *)& server1,(socklen_t *)&c))<0) 
                {
                    printf("not able to accpet\n");
                    return -1;
                }
                //create a process for server1
                no_of_clients++;
                sendfd(nsfd1,nsfd); // sending nsfd to s1
            }
            if(pfd[1].revents>0) // client request for server2
            {
                int nsfd,c=sizeof(server2);
                if((nsfd=accept(pfd[1].fd,(struct sockaddr *)&server2,(socklen_t *)&c))<0)
                {
                    printf("not able to accept\n");
                    return -1;
                }
                no_of_clients++;
                sendfd(nsfd2,nsfd); // sending nsfd to s2
                
            }
        }
    }

}
