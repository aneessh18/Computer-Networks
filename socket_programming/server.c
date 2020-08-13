#include <sys/socket.h>
#include <stdio.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <sys/select.h>
int max(int x,int y)
{
	return (x>y)? x:y;
}
int main()
{
	int tcp_fd,udp_fd;
	tcp_fd=socket(AF_INET,SOCK_STREAM,0);
	udp_fd=socket(AF_INET,SOCK_DGRAM,0);

	struct sockaddr_in server,client;

	server.sin_addr.s_addr=INADDR_ANY;
	server.sin_port=htons(8080);
	server.sin_family=AF_INET;

	if(bind(tcp_fd,(struct sockaddr *)&server,sizeof(server))<0)
	{
		puts("bind of tcp failed");
	}
	listen(tcp_fd,10);
	if(bind(udp_fd,(struct sockaddr *)&server,sizeof(server))<0)
	{
		puts("bind of udp failed");
	}

	fd_set read_set;
	FD_ZERO(&read_set);
	int nfds=max(tcp_fd,udp_fd)+1;// cause select will check from 0 to nfds-1
	while(1)
	{
		// setting server to read from clients
		FD_SET(tcp_fd,&read_set);
		FD_SET(udp_fd,&read_set);
		int r=select(nfds,&read_set,NULL,NULL,NULL);
		if(r>0)
		{
			if(FD_ISSET(tcp_fd,&read_set))
			{
				int l=sizeof(client);
				int tcp_nsfd=accept(tcp_fd,(struct sockaddr*)&client,&l);
				if(tcp_nsfd<0)
				{
					puts("accept failed");
				}
				if(fork()==0)
				{
					char buff[100];
					int sz=recv(tcp_nsfd,buff,100,0);
					buff[sz]='\0';
					puts(buff);
				}
				else
				{
					close(tcp_nsfd);
				}
				
			}
			if(FD_ISSET(udp_fd,&read_set))
			{
				if(fork()==0)
				{
					int l=sizeof(client);
					char buff[100];
					int sz=recvfrom(udp_fd,buff,100,0,(struct sockaddr*)&client,&l);
					buff[sz]='\0';
					puts(buff);
				}
				
			}
		}
	}

}