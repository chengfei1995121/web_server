#include"handle_request.h"
#include "http.h"
#include "error.h"
#include "respond.h"
#include "handle_file.h"
#include<errno.h>
#include "php_parse.h"
#define MAXSIZE 100000
#define ONEMAX 1024*1024
using namespace std;
void handle_request(int n)
{
	struct request_header H;
	struct stat sbuf;
	if(no_block(n)<0)
		return;
	H.fd=n;
	H.hd=(char *)malloc(ONEMAX);
	read_header(&H);
	//int l = read(n,H.hd, ONEMAX);
	//H.hd[l] = '\0';
	cout << H.hd << endl;//打印请求头
	getfileuri(&H);//获取文件路径
	//假如文件不存在，返回错误 stat函数用于读取文件信息
	cout<<"uri:"<<H.uri<<endl;
	get_request_method(&H);
	get_type(&H);
	if(strstr(H.method,"POST"))
	{
		get_postdata(&H);
	}
	if(stat(H.uri,&sbuf)<0)	
	{
		clienterror(H.fd);
		close(n);
		return;
	}	
	if(strstr(H.uri,".php"))
	{	
		respond_php(&H);
	}
	else 
	{
	if(!strcmp("GET",H.method))
	{
	H.filesize=sbuf.st_size;
	respond_static_html(&H);
	}
	}
	free(H.hd);
	close(n);
}
//响应静态内容
void respond_static_html(struct request_header *H)
{
	respond_header(H);
	respond_body(H);
}
//响应动态内容
void respond_php(struct request_header *H)
{
		char context[10000];
		memset(context,0,sizeof(context));
		handle_dynamic(H,context);
		H->filesize=strlen(context);
		respond_header(H);
		write(H->fd,context,strlen(context));
}
void read_header(struct request_header *H)
{
	int l,nread=0;
	while(1)
	{
		l=read(H->fd,H->hd+nread,ONEMAX-nread);
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
	H->hd[nread]='\0';
	return;
}
//非阻塞I/O
int no_block(int fd)
{
	int s=fcntl(fd,F_SETFL,O_NONBLOCK);
	if(s<0)
	{
		cout<<"fetcl"<<endl;
		return -1;
	}
	else 
		return 1;
}
