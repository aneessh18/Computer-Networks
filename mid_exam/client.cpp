#include <bits/stdc++.h>
#include <unistd.h>
#include <poll.h>
#include <fcntl.h>
#include <sys/msg.h>
#include <pthread.h>
#include <sys/ipc.h>
#include <sys/stat.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <sys/un.h>
using namespace std;
int main()
{
    int sfd = socket(AF_INET, SOCK_STREAM,0);

    struct sockaddr_in server ;
    server.sin_family = AF_INET;
    int port_no;
    cout<<"enter the port no \n";
    cin>>port_no;
    server.sin_port = htons(port_no);
    server.sin_addr.s_addr = INADDR_ANY;
    if(connect(sfd, (struct sockaddr *)&server, sizeof(server))<0)
    {
        perror("connect failed");
    }
    // receive msg from mod
    char buf[100];
    recv(sfd,buf,100,0);
    cout<<"the answer to the question is ---> "<<buf<<endl;

}