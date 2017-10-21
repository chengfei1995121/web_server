#include "http.h"
#include "respone.h"
#include "handle_file.h"
void respond(int n,char filename[],int filesize)
{
	//cout<<"1:"<<filesize<<endl;
	char type[1000];
	//将文件全部读到内存中
	/*ifstream cf(filename,std::ios::binary);
	char a[1000000];
	char type[1000];
	int chang;
	cf.seekg(0, ios::end);
	chang = cf.tellg();
	cf.seekg(0, ios::beg);
	cf.read(a, 1000000);
	cout <<"2:"<< strlen(a) << endl;*/
	//文件映射
	int stcd=open(filename,O_RDONLY,0);
	char *srcp;
	srcp=static_cast<char*>(mmap(0,filesize,PROT_READ,MAP_PRIVATE,stcd,0));
	close(stcd);
	//munmap(srcp,filesize);
	get_type(filename, type);//获取文件类型
	char b[1000] = "http/1.0 200 ok\r\nAccept-Ranges:bytes\r\nServer:cf web server\r\nContent-type:";
	strcat(b, type);
	
	strcat(b, "\r\n\r\n");
	write(n, b, strlen(b));
	//int slen = 0;
	write(n,srcp,filesize);
	munmap(srcp,filesize);
	//slen=send(n,a,chang,0);
	//cout << "sizeof:" << chang << endl;
	/*cout <<"leng:"<< cf.tellg() << endl;
	cout << "slen:" << slen << endl;*/
}
