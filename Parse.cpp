#include "http.h"
#include "error.h"
#include "Respond.h"
#include "handle_file.h"
#include<errno.h>
#include "php_parse.h"
#include "Parse.h"
#include "tool.h"
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
	//no_block();
	read_n(fd,hd);
	getfileuri();//获取文件路
	cout<<"uri:"<<uri<<endl;
	cout<<"fd"<<fd<<endl;
	get_request_method();
	get_type();
	if(strstr(method,"POST"))
	{
		get_postdata();
		get_content_length_and_type();
	}
	//假如文件不存在，返回错误 stat函数用于读取文件信息

	if(stat(uri,&sbuf)<0)	
	{
		clienterror(fd);
		close(fd);
		return;
	}	
	if(strstr(uri,".php"))
	{
		respond_php();
	}
	else 
	{
	if(!strcmp("GET",method))
	{
	filesize=sbuf.st_size;
	respond_static_html();
	}
	}
//	free(hd);
	Close();
}
//响应静态内容
void Parse::respond_static_html()
{
	respond_header(*this);
	respond_body(*this);
}
//响应动态内容
void Parse::respond_php()
{
		char context[10000];
		memset(context,0,sizeof(context));
		handle_dynamic(this,context);
		filesize=strlen(context);
		respond_header(*this);
		write(fd,context,strlen(context));
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
			cout<<"等待"<<endl;
			continue;
		}
		else 
			break;
	}
	hd[nread]='\0';
	return;
}
//非阻塞I/O
int Parse::no_block()
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
	strcat(uri,"./test/web-page");
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
void Parse::get_request_method()
{
	size_t i;
	for(i=0;i<strlen(hd);i++)
	{
		if(hd[i]==' ')
			break;
		method[i]=hd[i];
	}
	method[i]='\0';
}
void Parse::get_postdata()
{
	int len=strlen(hd);
	int k=0;
	for(int i=len-1;i>=0;i--)
	{
		if(hd[i]=='\n')
		{
			for(int j=i+1;j<len;j++)
			{
				post[k]=hd[j];
				k++;
			}
			break;
		}
	}
	post[k]='\0';
	cout<<"POST"<<post<<endl;
}
void Parse::get_content_length_and_type()
{
	int k=0,kk=0;
	for(size_t i=0;i<strlen(hd);i++)
	{
		if(hd[i]==':')
		{
			if(hd[i-1]=='h')
			{
				for(int j=i+1;hd[j]!='\r';j++)
				{
					if(hd[j]==' ')
						continue;
					content_length[k]=hd[j];
					k++;
				}
				content_length[k]='\0';
			}
			else 
			{
				if(hd[i-1]=='e')
				{
					for(int j=i+1;hd[j]!='\r';j++)
					{	if(hd[j]==' ')
							continue;
						content_type[kk]=hd[j];
						kk++;
					}

				content_type[kk]='\0';
				break;
				}
				
			}
		}
	}
//	cout<<"content_length"<<RH->content_length<<endl;
//	cout<<"content_type"<<RH->content_type<<endl;
}
void Parse::Close()
{
	free(hd);
	close(fd);
}
