#include "http.h"
#include "handle_request.h"
struct cf{
	int maxfd;//最大的文件描述符
	int maxi;
	int nready;
	int clientfd[1024];//已连接的
	fd_set read_set;
	fd_set ready_set;
}pool;
//初始化pool
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
//将描述符添加到pool里面
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
		pool.nready=select(pool.maxfd+1,&pool.ready_set,NULL,NULL,NULL);//阻塞
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
