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
 {NULL ,NULL}
};
void getfilename(struct request_header *RH)
{
	strcat(RH->uri,"./test");
	int k =6;
	for (size_t i = 0; i < strlen(RH->hd); i++)
	{
		if (RH->hd[i] == '/')
		{
			for (int j = i; RH->hd[j] != ' '; j++)
				RH->uri[k++] = RH->hd[j];
			break;
		}
	}
	RH->uri[k] = '\0';
	if(RH->uri[strlen(RH->uri)-1]=='/')
		strcat(RH->uri,"index.html");
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
