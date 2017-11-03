#ifndef hf
#define hf
struct filetype{
	const char *first;
	const char *second;
};
int getfileuri(struct request_header *RH);
void get_type(struct request_header *RH);
void get_request_method(struct request_header *RH);
void get_getdata(struct request_header *,int);
void get_postdata(struct request_header*);
int is_content_length(int start,struct request_header *RH);
void get_content_length_and_type(struct request_header*);
#endif
