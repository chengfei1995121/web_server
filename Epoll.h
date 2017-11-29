#ifndef __epoll__
#define __epoll__
#include "Socket.h"
#include<sys/epoll.h>
class Epoll{
	public:
		Epoll(int max);
		void epoll_open();
		void epoll_add(const Socket &Sk,int ident);
		void epoll_add(const Socket &Sk,int ident1,int ident2);
		void epoll_listen(const Socket &Sk);
		void epoll_close();
	private:
		int efd;
		int maxevent;
		struct epoll_event event;
		struct epoll_event *events;
};
int no_block(int fd);
void middle(int);
#endif
