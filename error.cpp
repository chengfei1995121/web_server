#include "http.h"
#include "error.h"
//错误处理
void clienterror(int fd)
{
	std::cout<<"yes"<<std::endl;
	char buf[1000]={"HTTP/1.0 404 NOT FOUND\r\nAccept-ranges:bytes\r\nContent-type:text/html\r\n\r\n"};
	write(fd,buf,strlen(buf));
	char a[1000]={"<html><head></head><body><p style='font-size:20px;'>404</p> NOT FOUND</body></html>"};
	write(fd,a,strlen(a));
}
