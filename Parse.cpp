#include "http.h"
#include "error.h"
#include "respond.h"
#include "handle_file.h"
#include<errno.h>
#include "php_parse.h"
#include "Parse.h"
#define MAXSIZE 100000
#define ONEMAX 1024*1024
using namespace std;


struct filetype ft[]={
 {".html", "text/html"},
 {".xml", "text/xml"},
 {".xhtml", "application/xhtml+xml"},
 {".txt", "text/plain"},
 {".rtf", "application/rtf"},
 {".pdf", "application/pdf"},
 {".word", "application/msword"},
 {".png", "image/png"},
 {".gif", "image/gif"},
 {".jpg", "image/jpeg"},
 {".jpeg", "image/jpeg"},
 {".au", "audio/basic"},
 {".mpeg", "video/mpeg"},
 {".mpg", "video/mpeg"},
 {".avi", "video/x-msvideo"},
 {".gz", "application/x-gzip"},
 {".tar", "application/x-tar"},
 {".css", "text/css"},
 {".js","application/javascript"},
 {".php","text/html"},
 {NULL ,NULL}
};

Parse::Parse(int n):fd(n){
	hd=(char *)malloc(ONEMAX*sizeof(char));
}
void Parse::handle_request()
{
	struct stat sbuf;
	read_header();
	cout << hd << endl;//打印请求头
	getfileuri();//获取文件路
	cout<<"uri:"<<uri<<endl;
	get_request_method(&H);
	get_type(&H);
	if(strstr(H.method,"POST"))
	{
		get_postdata(&H);
		get_content_length_and_type(&H);
	}
	//假如文件不存在，返回错误 stat函数用于读取文件信息

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
void Parse::read_header()
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
int Parse::getfileuri()
{
//	strcat(RH->uri,"./test/web-page");
//	int k =15;
	memset(uri,0,sizeof(uri));
	strcat(uri,"/home/chengfei/server/web_server/test");
	int k=strlen(uri);
	for (size_t i = 0; i < strlen(hd); i++)
	{
		if (hd[i] == '/')
		{
			for (int j = i; hd[j]!=' '; j++)
			{
				if(hd[j]=='?')
				{
					uri[k]='\0';
					get_getdata(j+1);
					return 1;
				}
				uri[k++] = hd[j];
			}
			break;
		}
	}
	uri[k] = '\0';
	if(uri[strlen(uri)-1]=='/')
		strcat(uri,"index.html");
	return 0;
}
//获取文件类型
void Parse::get_type()
{
	for(int i=0;ft[i].first!=NULL;i++)
	{
		if(strstr(uri,ft[i].first))
		{
			strcpy(filetype,ft[i].second);
			return;
		}

	}
}
void Parse::get_getdata(int start)
{
	int k=0;
	for(int i=start;hd[i]!=' ';i++)
	{
		getdata[k++]=hd[i];
	}
	getdata[k]='\0';
	cout<<"get数据"<<getdata<<endl;
}
