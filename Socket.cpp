#include"Socket.h"
#include<arpa/inet.h>
using namespace std;
Socket::Socket(const char *ip,const int port)
{
	server_addr.sin_port=htons(port);
	server_addr.sin_family=AF_INET;
	server_addr.sin_addr.s_addr=inet_addr(ip);
}
Socket::Socket(const int port)
{
	server_addr.sin_family=AF_INET;
	server_addr.sin_port=htons(port);
	server_addr.sin_addr.s_addr=htonl(INADDR_ANY);
}
int Socket::Socket_open()
{
	socketfd=socket(AF_INET,SOCK_STREAM,0);
	if(socketfd<0)
	{
		cout<<"socketfd fail"<<endl;
		return -1;
	}
	if(bind(socketfd,(sockaddr *)&server_addr,sizeof(server_addr))<0)
	{
		cout<<"bind fail"<<endl;
		return -1;
	}
	if(listen(socketfd,10)<0)
	{
		cout<<"listen fail"<<endl;
		return -1;
	}
	return 1;
}
void Socket::Socket_close()
{
	close(socketfd);
}
void Socket::no_block()
{
	if(fcntl(socketfd,F_SETFL,O_NONBLOCK)<0)
	{
		cout<<"fcntl fail"<<endl;
		exit(-1);
	}
}
void Socket::printf_fd()
{
	cout<<"socketfd:"<<socketfd<<endl;
}
