#include "php_parse.h"
#include<arpa/inet.h>
#include<malloc.h>
FCGI_Header makerequestheader(int type,int requestId,int contentLength,int paddingLength)
{
	FCGI_Header header;
	header.version=FCGI_VERSION;
	header.type=(unsigned char)type;
	header.requestIdB1=(unsigned char)((requestId>>8)&0xff);
	header.requestIdB0=(unsigned char)(requestId&0xff);
	header.contentLengthB1=(unsigned char)((contentLength>>8)&0xff);
	header.contentLengthB0=(unsigned char)((contentLength&0xff));
	header.paddingLength=(unsigned char)(paddingLength);
	header.reserved=0;
	return header;
}
FCGI_BeginRequestBody makebeginbody(int role)
{
	FCGI_BeginRequestBody body;
	body.roleB1=(unsigned char)((role>>8)&0xff);
	body.roleB0=(unsigned char)(role&0xff);
	body.flags=0;
	memset(body.reserved,0,sizeof(body.reserved));
	return body;
}
FCGI_BeginRequestRecord makebeginrecord(int REQUEST_ID)
{
	FCGI_BeginRequestRecord beginrecord;
	beginrecord.header=makerequestheader(FCGI_BEGIN_REQUEST,REQUEST_ID,sizeof(beginrecord.body),0);
	beginrecord.body=makebeginbody(FCGI_RESPONDER);
	return beginrecord;
}
bool makeNameValueBody(char *name,int nameLen,char *value,int valueLen,unsigned char *bodyptr,int *bodylenptr)
{
	unsigned char *start=bodyptr;
	if(nameLen<128)
	{
		*bodyptr++=(unsigned char)nameLen;
	}
	else 
	{
		*bodyptr++=(unsigned char)((nameLen>>24) | 0x80);
		*bodyptr++=(unsigned char)(nameLen>>16);
		*bodyptr++=(unsigned char)(nameLen>>8);
		*bodyptr++=(unsigned char)nameLen;
	}
	if(valueLen<128)
	{
		*bodyptr++=(unsigned char)valueLen;
	}
	else 
	{
		*bodyptr++=(unsigned char)((valueLen>>24) |0x80);
		*bodyptr++=(unsigned char)(valueLen>>16);
		*bodyptr++=(unsigned char)(valueLen>>8);
		*bodyptr++=(unsigned char)valueLen;
	}
	for(int i=0;i<nameLen;i++)
	{
		*bodyptr++=name[i];
	}
	for(int i=0;i<valueLen;i++)
	{
		*bodyptr++=value[i];
	}
	*bodylenptr=bodyptr-start;
	return true;
}
void sendparme(int fd,int id,char *name,char *value)
{
	FCGI_Header header;
	char request[1000];
	int bufflen=0;
	unsigned char buff[1000];
	bzero(buff,sizeof(buff));
	makeNameValueBody(name,strlen(name),value,strlen(value),buff,&bufflen);
	header=makerequestheader(FCGI_PARAMS,id,bufflen,0);
	memcpy(request,(char *)&header,REQUEST_HEADER_LEN);
	memcpy(request+REQUEST_HEADER_LEN,(char*)&buff,bufflen);
	int m=write(fd,request,bufflen+REQUEST_HEADER_LEN);
	printf("\n %d %s\n",m,buff);
	return;
}
/*int sendParamsRecord(
        int fd,
        char *name,
        int nlen,
        char *value,
        int vlen)
{
    unsigned char *buf, *old;
    int ret, pl,  cl = nlen + vlen;
    cl = (nlen < 128) ? ++cl : cl + 4; 
    cl = (vlen < 128) ? ++cl : cl + 4; 

    // 计算填充数据长度
    pl = (cl % 8) == 0 ? 0 : 8 - (cl % 8);
    old = buf = (unsigned char *)malloc(8 + cl + pl);

    FCGI_Header nvHeader = makerequestheader(4, 1, cl, pl);
    memcpy(buf, (char *)&nvHeader, 8);
    buf = buf + 8;

    if (nlen < 128) { // name长度小于128字节，用一个字节保存长度
        *buf++ = (unsigned char)nlen;
    } else { // 大于等于128用4个字节保存长度
        *buf++ = (unsigned char)((nlen >> 24) | 0x80);
        *buf++ = (unsigned char)(nlen >> 16);
        *buf++ = (unsigned char)(nlen >> 8);
        *buf++ = (unsigned char)nlen;
    }

    if (vlen < 128) { // value长度小于128字节，用一个字节保存长度
        *buf++ = (unsigned char)vlen;
    } else { // 大于等于128用4个字节保存长度
        *buf++ = (unsigned char)((vlen >> 24) | 0x80);
        *buf++ = (unsigned char)(vlen >> 16);
        *buf++ = (unsigned char)(vlen >> 8);
        *buf++ = (unsigned char)vlen;
    }

    memcpy(buf, name, nlen);
    buf = buf + nlen;
    memcpy(buf, value, vlen);

    ret = write(fd, old, 8 + cl + pl);

    free(old);

    if (ret == (8 + cl + pl)) {
        return 0;
    } else {
        return -1;
    }
}*/
void makeendrequest(int fd,int id)
{
	FCGI_Header endrecord;
	endrecord=makerequestheader(FCGI_PARAMS,id,0,0);
	write(fd,(char *)&endrecord,sizeof(endrecord));
}
void printf_html(char *context,char *htmltext)
{
	for(;*context!='\0';context++)
	{
		if(*context=='<')
		{
			while(*context!='\0')
			{
				*htmltext=*context;
				//printf("%c",*context);
				context++;
				htmltext++;
			}
			break;
		}
	}
}
int open_listent()
{
	int fd;
	struct sockaddr_in serveraddr;
	fd=socket(AF_INET,SOCK_STREAM,0);
	if(fd<0)
	{
		printf("socket error");
		exit(-1);
	}
	serveraddr.sin_family=AF_INET;
	serveraddr.sin_port=htons(9000);
	serveraddr.sin_addr.s_addr=inet_addr("127.0.0.1");
	int confd=connect(fd,(sockaddr *)&serveraddr,sizeof(serveraddr));
	if(confd==0)
	{
			printf("已连接 %d",confd);
	}
	else 
	{
			printf("失败 %d",confd);
			exit(-1);
	}
	return fd;
}
void printf_php_error(char *context,char *htmltext)
{
	strcpy(htmltext,context);
}
void handle_dynamic(char *uri,char *htmltext)
{
	int id=1;
	int fd=open_listent();
	FCGI_BeginRequestRecord record=makebeginrecord(id);
	FCGI_Header header;
	write(fd,(char *)&record,sizeof(record));
	char a[100]={"SCRIPT_FILENAME"};
	char b[100]={"/home/chengfei/server/web_server"};
	char c[100]={"REQUEST_METHOD"};
	char d[100]={"GET"};
	char e[100]={"QUERY_STRING"};
	char f[100]={"CF=good"};
	//strcat(b,uri);
	sendparme(fd,id,a,uri);
	sendparme(fd,id,c,d);
	sendparme(fd,id,e,f);
	makeendrequest(fd,id);
	char context[1000];
	int n=read(fd,&header,sizeof(header));
	printf("类型 %d\n",header.type);
	if(n>0)
	{
		if(header.type==6){
		int conlen=(header.contentLengthB1>>8)+header.contentLengthB0;
		printf("获取内容的长度 %d\n",conlen);
		int m=read(fd,context,conlen);
		context[m]='\0';
		printf("%s\n",context);
		printf_html(context,htmltext);
		}
		else 
		{
			if(header.type==7)
			{
				int conlen=(header.contentLengthB1>>8)+header.contentLengthB0;
				int m=read(fd,context,conlen);
				context[m]='\0';
				printf("%s\n",context);
				printf_php_error(context,htmltext);
			}
		}
	}
	else 
	{
		printf("长度 %d",n);
	}
	close(fd);
}
