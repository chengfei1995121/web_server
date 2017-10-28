#include"handle_request.h"
#include "http.h"
#include "error.h"
#include "respond.h"
#include "handle_file.h"
#include<errno.h>
#define MAXSIZE 100000
#define ONEMAX 1024*1024
using namespace std;
void handle_request(int n)
{
	fcntl(n,F_SETFL,O_NONBLOCK);
	struct request_header H;
	struct stat sbuf;
	H.fd=n;
	H.hd=(char *)malloc(ONEMAX);
	read_header(&H);
	//int l = read(n,H.hd, MAXSIZE);
	//H.hd[l] = '\0';
	cout << H.hd << endl;//打印请求头
	getfilename(&H);//获取文件名
	//cout<<filename<<endl;
	//假如文件不存在，返回错误 stat函数用于读取文件信息
	if(stat(H.uri,&sbuf)<0)	
	{
		clienterror(H.fd);
		return;
		
	}
	get_request_method(&H);
	get_type(&H);
	if(!strcmp("GET",H.method)){
	//cout << filename << endl;
	H.filesize=sbuf.st_size;
	respond(&H);//响应
	free(H.hd);
	}
}
void read_header(struct request_header *H)
{
	int l,nread=0;
	while(1)
	{
		l=read(H->fd,H->hd+nread,ONEMAX-nread);
		if(l>0)
		{
			cout<<l<<endl;
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
	H->hd[nread]='\0';
	return;
}
