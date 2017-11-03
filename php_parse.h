#ifndef __FCGI__
#define __FCGI__
#include "http.h"
#define REQUEST_HEADER_LEN 8

#define FCGI_VERSION 1

//type类型
#define FCGI_BEGIN_REQUEST 1
#define FCGI_END_REQUEST 3
#define FCGI_PARAMS 4
#define FCGI_STDIN 5
#define FCGI_STDOUT 6
#define FCGI_STDERR 7
#define FCGI_DATA 8
typedef struct{
	unsigned char version;
	unsigned char type;
	unsigned char requestIdB1;
	unsigned char requestIdB0;
	unsigned char contentLengthB1;
	unsigned char contentLengthB0;
	unsigned char paddingLength;
	unsigned char reserved;
}FCGI_Header;
//role类型
#define FCGI_RESPONDER 1
typedef struct{
	unsigned char roleB1;
	unsigned char roleB0;
	unsigned char flags;
	unsigned char reserved[5];
}FCGI_BeginRequestBody;

typedef struct{
	FCGI_Header header;
	FCGI_BeginRequestBody body;
}FCGI_BeginRequestRecord;

typedef struct{
	unsigned char appStatusB3;
	unsigned char appStatusB2;
	unsigned char appStatusB1;
	unsigned char appStatusB0;
	unsigned char protocolStatus;
	unsigned char reserved[3];
}FCGI_EndRequestBody;
typedef struct{
	FCGI_Header header;
	FCGI_EndRequestBody body;

}FCGI_EndRequestRecord;
FCGI_Header makerequestheader(int,int,int,int);
FCGI_BeginRequestBody makebeginbody(int);
FCGI_BeginRequestRecord makebeginrecord(int);
bool makeNameValueBody(char*,int,char*,int,unsigned char*,int*);
void sendparme(int,int,char*,char*);
void makeendrequest(int,int,int);
void pritf_html(char *context);
int open_listent();
void handle_dynamic(struct request_header*,char*);
#endif
