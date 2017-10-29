#include "http.h"
#include "respond.h"
#include "handle_file.h"
#include "handle_request.h"
#include<errno.h>
#define B_SIZE 10000
using namespace std;
void respond(struct request_header *RH)
{
	//cout<<"1:"<<filesize<<endl;
	//char type[1000];
	//将文件全部读到内存中
	/*ifstream cf(filename,std::ios::binary);
	char a[1000000];
	char type[1000];
	int chang;
	cf.seekg(0, ios::end);
	chang = cf.tellg();
	cf.seekg(0, ios::beg);
	cf.read(a, 1000000);
	cout <<"2:"<< strlen(a) << endl;
	*/
	//cout<<"uri:"<<RH->uri<<endl;
	respond_header(RH);
	respond_body(RH);

	//slen=send(n,a,chang,0);
	//cout << "sizeof:" << chang << endl;
	/*cout <<"leng:"<< cf.tellg() << endl;
	cout << "slen:" << slen << endl;*/
}
//返回头部消息
void respond_header(struct request_header *RH)
{
	char buf[B_SIZE]="http/1.1 200 ok\r\nAccept-Ranges:bytes\r\nServer:cf web server\r\nContent-type:";
	sprintf(buf,"%s%s\r\nContent-length:%d\r\n",buf,RH->filetype,RH->filesize);
	strcat(buf,"Connection:keep-alive\r\n\r\n");
	write(RH->fd,buf,strlen(buf));
}
//返回主体内容
void respond_body(struct request_header *RH)
{
	//文件映射
	int stcd=open(RH->uri,O_RDONLY,0);
	if(stcd<0)
		cout<<"open"<<endl;
	char *srcp;
	int w,nwrite=0;
	srcp=static_cast<char*>(mmap(0,RH->filesize,PROT_READ,MAP_PRIVATE,stcd,0));
	close(stcd);
	while((w=write(RH->fd,srcp+nwrite,RH->filesize))!=0)
	{
		if(w==-1)
		{
			if(errno==EINTR)
				continue;
			else
			{
				cout<<"write"<<endl;
				break;
			}
		}
		nwrite+=w;
		if(nwrite==RH->filesize)
			break;
	}
	munmap(srcp,RH->filesize);
}
