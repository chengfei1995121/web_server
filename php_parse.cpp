#include "php_parse.h"
#include<arpa/inet.h>
FCGI_Header makerequestheader(int type,int requestId,int contentLength,int paddingLength)
{
	FCGI_Header header;
	header.version=1;
	header.type=(unsigned char)type;
	header.requestIdB1=(unsigned char)((requestId>>8)&0xff);
	header.requestIdB0=(unsigned char)(requestId&0xff);
	header.contentLengthB1=(unsigned char)((contentLength>>8)&0xff);
	header.contentLengthB0=(unsigned char)((contentLength&0xff));
	header.paddingLength=(unsigned char)(paddingLength);
	header.reserved=0;
	return header;
}
FCGI_BeginRequestBody makeBB(int role)
{
	FCGI_BeginRequestBody bb;
	bb.roleB1=(unsigned char)((role>>8)&0xff);
	bb.roleB0=(unsigned char)(role&0xff);
	bb.flags=0;
	memset(bb.reserved,sizeof(bb.reserved),0);
	return bb;
}
FCGI_BeginRequestRecord makerecord()
{
	FCGI_BeginRequestRecord BR;
	BR.header=makerequestheader(1,1,sizeof(BR.header),0);
	BR.body=makeBB(1);
	return BR;
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
			*bodyptr++=(unsigned char)((nameLen>>24) |0x80);
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
int main()
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
	char b[100];
	//printf("2\n");
	int confd=connect(fd,(sockaddr *)&serveraddr,sizeof(serveraddr));
	//printf("3\n");
	if(confd==0)
	{
			printf("已连接 %d",confd);
			//exit(-1);
	}
	else 
	{
			printf("失败 %d",confd);
			//exit(-1);
	}
	FCGI_BeginRequestRecord record=makerecord();
	FCGI_Header header;
	int l=write(fd,(char *)&record,sizeof(record));
	printf("写入的长度 %d %d\n",l,sizeof(record));
	int n=read(fd,(char *)&header,100);
	if(n>0)
	{printf("长度 %d",n);
	b[n]='\0';
	printf("%s",b);
	}
	else 
	{
		printf("长度 %d",n);
	}
	close(confd);
	close(fd);
}
