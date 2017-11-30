#ifndef rs 
#define rs
#include "Parse.h"
//void respond(struct request_header *RH);
void respond_header(const Parse p);
void respond_body(const Parse p);
#endif
