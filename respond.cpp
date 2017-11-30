#include "http.h"
#include "handle_file.h"
#include<errno.h>
#include"Respond.h"
#define B_SIZE 10000
using namespace std;
void respond()
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
//	respond_header(RH);
//	respond_body(RH);

	//slen=send(n,a,chang,0);
	//cout << "sizeof:" << chang << endl;
	/*cout <<"leng:"<< cf.tellg() << endl;
	cout << "slen:" << slen << endl;*/
}
//返回头部消息
void respond_header(const Parse p)
{
//	cout<<"content_type"<<RH->filetype<<"  filesize"<<RH->filesize<<endl;
	char buf[B_SIZE]="http/1.0 200 ok\r\nAccept-Ranges:bytes\r\nServer:cf web server\r\nContent-type:";
	sprintf(buf,"%s%s\r\nContent-length:%d\r\n\r\n",buf,p.filetype,p.filesize);
	//strcat(buf,"Connection:close\r\n\r\n");
	write(p.fd,buf,strlen(buf));
}
//返回主体内容
void respond_body(const Parse p)
{
	//文件映射
	int stcd=open(p.uri,O_RDONLY,0);
	if(stcd<0)
		cout<<"open mmap failed"<<endl;
	char *srcp;
	int w,nwrite=0;
	srcp=static_cast<char*>(mmap(0,p.filesize,PROT_READ,MAP_PRIVATE,stcd,0));
	close(stcd);
	while((w=write(p.fd,srcp+nwrite,p.filesize))!=0)
	{
		if(w==-1)
		{
			if(errno==EINTR)
				continue;
			else
			{
				if(errno==EAGAIN)
				{
					cout<<"没有空间"<<endl;
					continue;
				}
				else
				{
				cout<<"respond write faile"<<endl;
				break;
				}
			}
		}
		nwrite+=w;
		if(nwrite==p.filesize)
			break;
	}
	munmap(srcp,p.filesize);
}
