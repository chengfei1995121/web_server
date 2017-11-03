#ifndef hr
#define hr
#define H_SIZE 100
struct request_header{
	char *hd;//请求头内容
	char method[H_SIZE];//请求类型
	char uri[H_SIZE];//请求uri
	char filetype[H_SIZE];
	int fd;//连接描述符
	int filesize;//文件大小
	char getdata[H_SIZE];
	char post[H_SIZE];
	char content_length[16];
	char content_type[H_SIZE];
};
void handle_request(int n);
void read_header(struct request_header *);
void respond_php(struct request_header *);
void respond_static_html(struct request_header *);
int no_block(int fd);
#endif
