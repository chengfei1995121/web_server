#ifndef hr
#define hr
struct request_header{
	char hd[10000];//请求头内容
	char method[100];//请求类型
	char filename[1000];//请求文件名
};
void handle_request(int n);
void get_request_method(char*,char*);
#endif
