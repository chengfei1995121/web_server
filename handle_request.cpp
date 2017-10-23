#include"handle_request.h"
#include "http.h"
#include "error.h"
#include "respond.h"
#include "handle_file.h"
#define MAXSIZE 100000
using namespace std;
void handle_request(int n)
{
	struct request_header H;
	struct stat sbuf;
	H.fd=n;
	int l = read(n,H.hd, MAXSIZE);
	H.hd[l] = '\0';
	cout << H.hd << endl;//打印请求头
	getfilename(&H);//获取文件名
	get_request_method(&H);
	get_type(&H);
	//cout<<filename<<endl;
	//假如文件不存在，返回错误 stat函数用于读取文件信息
	if(stat(H.uri,&sbuf)<0)	
	{
		clienterror(H.fd);
		return;
		
	}
	if(!strcmp("GET",H.method)){
	//cout << filename << endl;
	H.filesize=sbuf.st_size;
	respond(&H);//响应
	}
}
