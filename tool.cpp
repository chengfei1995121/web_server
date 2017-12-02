#include "http.h"
#include<errno.h>
#include<sys/epoll.h>
#include "Parse.h"
#define ONEMAX 10000
using namespace std;

int read_n(int fd,char *hd)
{
	int l,nread=0;
	while(1)
	{
		l=read(fd,hd+nread,ONEMAX-nread);
		if(l>0)
		{
			nread+=l;
		}
		else 
		{
			if(l==0)
			{
				break;
			}
			else
			{
				if(l==-1&&errno==EINTR)
				{
					continue;
				}
				else
					if(l==-1&&errno==EAGAIN&&nread==0)
						return 0;
					else 
						break;
			}
		}
	}
	hd[nread]='\0';
	//	cout<<"头部"<<hd<<endl;
	//	cout<<"大小"<<nread;
	if(nread>0)
		return 1;
	else 
		return 2;
}
void write_n(int fd,char *context,int size)
{
	int nwrite=0;
	int w;
	while((w=write(fd,context+nwrite,size))!=0)
	{
		if(w==-1)
		{
			if(errno==EINTR)
				continue;
			else
			{
				if(errno==EAGAIN)
				{
					cout<<"EAGIN"<<endl;
					continue;
				}
				else
				{
				break;
				}
			}
		}
		nwrite+=w;
		if(nwrite==size)
			break;
	}
}
int set_nonblocking(int fd)
{
	int flags=fcntl(fd,F_GETFL,0);
	if(flags==-1)
		flags=0;
	return fcntl(fd,F_SETFL,flags | O_NONBLOCK);
}

void middle(int fd,int efd)
{
	struct epoll_event event;
	Parse P(fd);
	if(!P.handle_request())
	{
		event.data.fd=fd;
		event.events=EPOLLOUT;
		epoll_ctl(efd,EPOLL_CTL_DEL,fd,&event);
		P.Close();
	}			
}
