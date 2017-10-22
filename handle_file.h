#ifndef hf
#define hf
struct filetype{
	const char *first;
	const char *second;
};
void getfilename(struct request_header *RH);
void get_type(struct request_header *RH);
void get_request_method(struct request_header *RH);
#endif
