#include<sys/socket.h>
#include<sys/types.h>
#include<netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include<iostream>
#include<fstream>
#include<string>
#include<cstring>
#include<vector>
#include<map>
#include<unistd.h>
#include<signal.h>
#include<sys/wait.h>
#include<sys/stat.h>
#include<sys/mman.h>
#include<sys/fcntl.h>
#include<sys/stat.h>
using namespace std;
//获取请求文件名
void getfilename(char a[],char filename[])
{
	int k =6;
	for (size_t i = 0; i < strlen(a); i++)
	{
		if (a[i] == '/')
		{
			for (int j = i; a[j] != ' '; j++)
				filename[k++] = a[j];
			break;
		}
	}
	filename[k] = '\0';
	if(filename[strlen(filename)-1]=='/')
		strcat(filename,"index.html");
}
//获取请求类型
void get_type(char filename[], char type[])
{
	if (strstr(filename, ".html"))
		strcpy(type, "text/html");
	else
	{
		if (strstr(filename, ".gif"))
			strcpy(type, "image/gif");
		else
			if (strstr(filename, ".jpg"))
				strcpy(type, "image/jpeg");
			else
				if (strstr(filename, ".css"))
					strcpy(type, "text/css");
				else
					if (strstr(filename, ".png"))
						strcpy(type, "image/png");
					else
						strcpy(type, "text/plain");
	}
}
//响应
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
	get_type(filename, type);
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
//错误处理
void clienterror(int fd)
{
	char buf[1000]={"HTTP/1.0 404 NOT FOUND\r\n\r\n"};
	write(fd,buf,strlen(buf));
}
//处理请求
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
//信号处理器,多进程并发
/*void handler(int sign)
{
	//调用waitpid可以不用阻塞
	while(waitpid(-1,0,WNOHANG)>0)
	{
		cout<<"子进程退出"<<endl;
	}
	return;
}*/
//I/O多路复用
struct cf{
	int maxfd;
	int maxi;
	int nready;
	int clientfd[1024];
	fd_set read_set;
	fd_set ready_set;
}pool;
void init(int listenfd,struct cf *pool)
{
	pool->maxi=-1;
	for(int i=0;i<1024;i++)
		pool->clientfd[i]=-1;
	pool->maxfd=listenfd;
	FD_ZERO(&pool->read_set);
	FD_SET(listenfd,&pool->read_set);
	return;
}
void add_pool(int confd,struct cf *pool)
{
	int i;
	pool->nready--;
	for(i=0;i<1024;i++)
	{
		if(pool->clientfd[i]<0)
		{
			pool->clientfd[i]=confd;
			FD_SET(confd,&pool->read_set);
			if(pool->maxfd<confd)
				pool->maxfd=confd;
			if(pool->maxi<i)
				pool->maxi=i;
			break;
		}
	}
}
int main()
{
	struct sockaddr_in client, server;
	int socketd = socket(AF_INET, SOCK_STREAM, 0);
	server.sin_family = AF_INET;
	server.sin_port = htons(8888);
	server.sin_addr.s_addr =htonl(INADDR_ANY);
	bind(socketd, (sockaddr*)&server, sizeof(server));
	listen(socketd, 10);
	socklen_t l = sizeof(client);
	//注册信号处理器，捕获子进程退出信号
	/*if(signal(SIGCHLD,handler)==SIG_ERR)
	{
		printf("signal() failed\n");
		return -1;
	}*/
	init(socketd,&pool);

	while (1) {
		/***I/O多路复用并发***/
		int confd;
		pool.ready_set=pool.read_set;
		pool.nready=select(pool.maxfd+1,&pool.ready_set,NULL,NULL,NULL);
		if(FD_ISSET(socketd,&pool.ready_set))
		{
			confd=accept(socketd,(sockaddr*)&client,&l);
			add_pool(confd,&pool);
		}
		for(int i=0;i<1024&&pool.nready>0;i++)
		{
			confd=pool.clientfd[i];
			if(confd>0&&FD_ISSET(confd,&pool.ready_set))
			{
				handle_request(confd);
				pool.clientfd[i]=-1;
				FD_CLR(confd,&pool.read_set);
				close(confd);
			}
		}
		/****end***/

		/****多进程并发*****/
		/*int n = accept(socketd, (sockaddr*)&client, &l);
		int p_id=fork();
		if(p_id==0)
		{
			close(socketd);//因为创建子进程 socketd文件引用计数会加1，如果不关闭，最后就无法实现完全关闭，下面的n也是一样
			handle_request(n);
			close(n);
			exit(0);
		}
		close(n);*/
		/****end*/
	}
	close(socketd);
}
/*int main()
{
	char a[100] = "E://www/fav.jpg";
	char c[100] = "E://www/wo.jpg";
	ifstream cf(a,ios::binary);
	int i = 0;
	if (cf.is_open())
	{
		char b[100000];
		cf.read(b, 100000);
		ofstream aa(c, ios::binary);
		if (aa.is_open())
		{
			aa.write(b, 100000);
		}
	}
	else
	{
		cout << "shibai" << endl;
	}
}*/
