#include"threadpool.h"
using namespace std;
static Thread_pool *pool=NULL;
void pool_init(int m)
{
	pool=(Thread_pool *)malloc(sizeof(Thread_pool));
	pthread_mutex_init(&(pool->queue_lock),NULL);
	pthread_cond_init(&(pool->queue_ready),NULL);
	pool->queue_head=NULL;
	pool->max_thread_num=m;
	pool->threadid=(pthread_t *)malloc(sizeof(pthread_t)*m);
	pool->cur_queue_size=0;
	for(int i=0;i<m;i++)
		pthread_create(&(pool->threadid[i]),NULL,thread_process,NULL);
}
int pool_add(void*(*func) (void *arg),void *arg)
{
	Thread_task *new_task=(Thread_task *)malloc(sizeof(Thread_task));
	new_task->func=func;
	new_task->arg=arg;
	new_task->next=NULL;
	pthread_mutex_lock(&(pool->queue_lock));
	Thread_task *mumber=pool->queue_head;
	while(mumber->next!=NULL)
		mumber=mumber->next;
	mumber->next=new_task;
	pool->cur_queue_size++;
	pthread_mutex_unlock(&(pool->queue_lock));
	if(pool->cur_queue_size>0)
	{
		pthread_cond_signal(&(pool->queue_ready));
	}
	return 0;
}
void *thread_process(void *arg)
{
	while(1)
	{
		pthread_mutex_lock(&(pool->queue_lock));
		if(pool->cur_queue_size==0)
			pthread_cond_wait(&(pool->queue_ready),&(pool->queue_lock));
		pool->cur_queue_size--;
		Thread_task *worker=pool->queue_head;
		pool->queue_head=worker->next;
		pthread_mutex_unlock(&(pool->queue_lock));
		(*worker->func)(worker->arg);
		free(worker);
	}
}
