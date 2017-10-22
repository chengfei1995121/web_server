#include "http.h"
#include "respond.h"
#include "handle_file.h"
#include "handle_request.h"
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
	char buf[B_SIZE]="http/1.0 200 ok\r\nAccept-Ranges:bytes\r\nServer:cf web server\r\nContent-type:";
	sprintf(buf,"%s%s\r\nContent-length:%d\r\n\r\n",buf,RH->filetype,RH->filesize);
	write(RH->fd,buf,strlen(buf));
}
//返回主体内容
void respond_body(struct request_header *RH)
{
	//文件映射
	int stcd=open(RH->filename,O_RDONLY,0);
	char *srcp;
	srcp=static_cast<char*>(mmap(0,RH->filesize,PROT_READ,MAP_PRIVATE,stcd,0));
	close(stcd);
	write(RH->fd,srcp,RH->filesize);
	munmap(srcp,RH->filesize);
}
