#include "http.h"
#include "Epoll.h"
#include "Parse.h"
#include "threadpool.h"
#include "tool.h"
using namespace std;
Epoll::Epoll(int max):maxevent(max){}
void Epoll::epoll_open()
{
	efd=epoll_create1(0);
	if(efd<0)
	{
		cout<<"epoll create fail"<<endl;
		exit(-1);
	}
	events=new struct epoll_event[maxevent];
	if(events==NULL)
	{
		cout<<"events fail"<<endl;
		exit(-1);
	}
}
void Epoll::epoll_add(const Socket &Sk,int ident)
{
	event.data.fd=Sk.socketfd;
	event.events=ident;
	if(epoll_ctl(efd,EPOLL_CTL_ADD,Sk.socketfd,&event)<0)
	{

		cout<<"epoll_ctl fail"<<endl;
		exit(-1);
	}
}
void Epoll::epoll_add(const Socket &Sk,int ident1,int ident2)
{
	event.data.fd=Sk.socketfd;
	event.events=ident1 | ident2;
	if(epoll_ctl(efd,EPOLL_CTL_ADD,Sk.socketfd,&event)<0)
	{
		cout<<"epol_ctl fail"<<endl;
		exit(-1);
	}
}
void Epoll::epoll_listen(const Socket &Sk)
{
	while (1) 
	{
		int confd;
		socklen_t l=sizeof(Sk.client_addr);
		/***I/O多路复用 epoll***/
		int num=epoll_wait(efd,events,maxevent,-1);
		for(int i=0;i<num;i++)
		{
			if(Sk.socketfd==events[i].data.fd)
			{
				confd=accept(Sk.socketfd,(struct sockaddr*)&Sk.client_addr,&l);
				cout<<"confd "<<confd<<endl;
				set_nonblocking(confd);
				event.data.fd=confd;
				event.events=EPOLLIN | EPOLLET;
				if(epoll_ctl(efd,EPOLL_CTL_ADD,confd,&event)<0)
				{
					cout<<"ctl error"<<endl;
					exit(-1);
				}

			}
			else 
			{
				if((events[i].events&EPOLLIN)&&events[i].data.fd>0)
				{
					confd=events[i].data.fd;
					Parse P(confd);
					if(!P.handle_request())
						//pool_add(middle,confd);
					{
						event.data.fd=confd;
						event.events=EPOLLOUT;
						epoll_ctl(efd,EPOLL_CTL_DEL,confd,&event);
					}
				}

			}

		}
	}
}
void Epoll::epoll_close()
{
	delete []events;
	close(efd);
}
void middle(int fd)
{
	Parse P(fd);
	P.handle_request();	
}
