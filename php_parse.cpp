#include "php_parse.h"
#include<arpa/inet.h>
#include<malloc.h>
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
	memset(bb.reserved,0,sizeof(bb.reserved));
	return bb;
}
FCGI_BeginRequestRecord makerecord()
{
	FCGI_BeginRequestRecord BR;
	BR.header=makerequestheader(1,1,sizeof(BR.body),0);
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
void sendparme(int fd,char *name,char *value)
{
	FCGI_Header header;
	char request[1000];
	int bufflen=0;
	//int n=write(fd,(char *)&header,sizeof(header));
	//memcpy(request,(char *)&header,8);
	unsigned char buff[1000];
	bzero(buff,sizeof(buff));
	makeNameValueBody(name,strlen(name),value,strlen(value),buff,&bufflen);
	//n=write(fd,buff,bufflen);
	header=makerequestheader(4,1,bufflen,0);
	//memcpy(request,(char *)&header,8);
	//memcpy(request+8,buff,bufflen);
	//printf("字符 %s\n",request);
	//int n=write(fd,(char *)&header,sizeof(header));
	write(fd,(char *)&header,8);
	int m=write(fd,(char *)&buff,bufflen);
	//printf("字符 %s\n",request);
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
void makeendrequest(int fd)
{
	FCGI_Header endrecord;
	endrecord=makerequestheader(4,1,0,0);
	write(fd,(char *)&endrecord,sizeof(endrecord));
}
void printf_html(char *context)
{
	for(;*context!='\0';context++)
	{
		if(*context=='<')
		{
			while(*context!='\0')
				printf("%c",*context);
			break;
		}
	}
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
			exit(-1);
	}
	FCGI_BeginRequestRecord record=makerecord();
	FCGI_Header header;
	int l=write(fd,(char *)&record,sizeof(record));
	char a[100]={"SCRIPT_FILENAME"};
	char b[100]={"/home/chengfei/index.php"};
	char c[100]={"REQUEST_METHOD"};
	char d[100]={"GET"};
	sendparme(fd,a,b);
	sendparme(fd,c,d);
	makeendrequest(fd);
	int n=read(fd,&header,sizeof(header));
	char context[1000];
	printf("类型 %d\n",header.type);
	if(n>0)
	{
		if(header.type==6){
		int conlen=(header.contentLengthB1>>8)+header.contentLengthB0;
		printf("获取内容的长度 %d\n",conlen);
		read(fd,context,conlen);
		printf("%s\n",context);
		//printf_html(context);
		}
	}
	else 
	{
		printf("长度 %d",n);
	}
	close(confd);
	close(fd);
}
