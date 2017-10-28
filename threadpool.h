#ifndef TP 
#define TP
#include "http.h"
#include<pthread.h>
typedef struct task{
	void *(*func) (void *arg);
	void *arg;
	struct task *next;
}Thread_task;
typedef struct pool{
	pthread_mutex_t queue_lock;
	pthread_cond_t queue_ready;
	Thread_task *queue_head;
	pthread_t *threadid;
	int max_thread_num;
	int cur_queue_size;
}Thread_pool;
void pool_init(int m);
int pool_add(void*(*func) (void *arg),void *arg);
void *thread_process(void *arg);
#endif
