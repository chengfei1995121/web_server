#include "Socket.h"
using namespace std;
int main()
{
	Socket first("127.0.0.1",8888);
	first.Socket_open();
	first.printf_fd();
}
