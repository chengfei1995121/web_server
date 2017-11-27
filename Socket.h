#ifndef __SOCKET
#define __SOCKET
#include"http.h"
#include<string>
class Socket{
	public:
		friend class Epoll;
		Socket(const char *ip,const int port);
		Socket(const int port);
		int Socket_open();
		void Socket_close();
		void printf_fd();
	private:
		struct sockaddr_in server_addr;
		struct sockaddr_in client_addr;
		int socketfd;
		int confd;
};
#endif
