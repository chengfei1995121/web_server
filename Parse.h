#ifndef __PARSE__
#define __PARSE__
#define H_SIZE 1024
class Parse{
	public:
		friend void respond_body(const Parse&);
		friend void respond_header(const Parse&);
		Parse(int n);
		void read_header();
		int getfileuri();
		void get_type();
		void get_getdata(int);
		void get_request_method();
		void  get_postdata();
		void get_content_length_and_type();
		void respond_static_html();
		void handle_request();
		void Close();
	private:
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
