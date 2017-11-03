#include "http.h"
#include "handle_file.h"
#include "handle_request.h"
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
int getfileuri(struct request_header *RH)
{
//	strcat(RH->uri,"./test/web-page");
//	int k =15;
	memset(RH->uri,0,sizeof(RH->uri));
	strcat(RH->uri,"/home/chengfei/server/web_server/test");
	int k=strlen(RH->uri);
	for (size_t i = 0; i < strlen(RH->hd); i++)
	{
		if (RH->hd[i] == '/')
		{
			for (int j = i; RH->hd[j]!=' '; j++)
			{
				if(RH->hd[j]=='?')
				{
					RH->uri[k]='\0';
					get_getdata(RH,j+1);
					return 1;
				}
				RH->uri[k++] = RH->hd[j];
			}
			break;
		}
	}
	RH->uri[k] = '\0';
	if(RH->uri[strlen(RH->uri)-1]=='/')
		strcat(RH->uri,"index.html");
	return 0;
}
//获取请求类型
void get_type(struct request_header *RH)
{
	for(int i=0;ft[i].first!=NULL;i++)
	{
		if(strstr(RH->uri,ft[i].first))
		{
			strcpy(RH->filetype,ft[i].second);
			return;
		}

	}
}
void get_request_method(struct request_header *RH)
{
	size_t i;
	for(i=0;i<strlen(RH->hd);i++)
	{
		if(RH->hd[i]==' ')
			break;
		RH->method[i]=RH->hd[i];
	}
	RH->method[i]='\0';
}
void get_getdata(struct request_header *RH,int start)
{
	int k=0;
	for(int i=start;RH->hd[i]!=' ';i++)
	{
		RH->getdata[k++]=RH->hd[i];
	}
	RH->getdata[k]='\0';
	cout<<RH->getdata<<endl;
}
void get_postdata(struct request_header *RH)
{
	int len=strlen(RH->hd);
	int k=0;
	for(int i=len-1;i>=0;i--)
	{
		if(RH->hd[i]=='\n')
		{
			for(int j=i+1;j<len;j++)
			{
				RH->post[k]=RH->hd[j];
				k++;
			}
			break;
		}
	}
	RH->post[k]='\0';
	cout<<"POST"<<RH->post<<endl;
}
