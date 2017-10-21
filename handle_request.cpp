#include"handle_request.h"
#include "http.h"
#include "error.h"
#include "respone.h"
#include "handle_file.h"
#define MAXSIZE 100000
using namespace std;
void handle_request(int n)
{
	struct request_header H;
	struct stat sbuf;
	int l = read(n,H.hd, MAXSIZE);
	H.hd[l] = '\0';
	cout << H.hd << endl;//打印请求头
	strcat(H.filename,"./test");
	getfilename(H.hd, H.filename);//获取文件名
	get_request_method(H.hd,H.method);
	//cout<<filename<<endl;
	//假如文件不存在，返回错误 stat函数用于读取文件信息
	if(stat(H.filename,&sbuf)<0)	
	{
		clienterror(n);
		return;
		
	}
	if(!strcmp("GET",H.method)){
	//cout << filename << endl;
	respond(n,H.filename,sbuf.st_size);//响应
	}
}
//处理请求类型
void get_request_method(char a[],char type[])
{
	size_t i;
	for(i=0;i<strlen(a);i++)
	{
		if(a[i]==' ')
			break;
		type[i]=a[i];
	}
	type[i]='\0';
}
