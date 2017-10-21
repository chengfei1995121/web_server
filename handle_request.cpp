#include<iostream>
#include<string>
#include<string.h>
#include"handle_request.h"
#include "http.h"
#include "error.h"
#include "respone.h"
#include "handle_file.h"
using namespace std;
void handle_request(int n)
{
	char a[10000];
	struct stat sbuf;
	int l = read(n, a, 10000);
	a[l] = '\0';
	cout << a << endl;//打印请求头
	char filename[1000];
	strcat(filename,"./test");
	getfilename(a, filename);//获取文件名
	//cout<<filename<<endl;
	//假如文件不存在，返回错误 stat函数用于读取文件信息
	if(stat(filename,&sbuf)<0)	
	{
		clienterror(n);
		return;
		
	}
	//cout << filename << endl;
	respond(n,filename,sbuf.st_size);//响应
}

