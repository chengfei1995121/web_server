#include "http.h"
#include<errno.h>
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
		else if(l==0)
		{
			break;
		}
		else if(l==-1&&errno==EINTR)
		{
			continue;
		}
		else 
			break;
	}
	hd[nread]='\0';
//	cout<<"头部"<<hd<<endl;
//	cout<<"大小"<<nread;
	if(nread==0)
		return 0;
	else 
		return 1;
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
				cout<<"respond write faile"<<endl;
				break;
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
