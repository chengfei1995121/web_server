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
int pool_add(void (*func) (int m,int efd),int n,int efd)
{
	Thread_task *new_task=(Thread_task *)malloc(sizeof(Thread_task));
	if(new_task==NULL)
	{
		cout<<"malloc fail"<<endl;
		return -1;
	}
	new_task->func=func;
	new_task->fd=n;
	new_task->efd=efd;
	new_task->next=NULL;
	pthread_mutex_lock(&(pool->queue_lock));
	Thread_task *mumber=pool->queue_head;
	if(mumber!=NULL){
	while(mumber->next!=NULL)
		mumber=mumber->next;
	mumber->next=new_task;
	}
	else 
		pool->queue_head=new_task;
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
		//记得用while
		while(pool->cur_queue_size==0)
			pthread_cond_wait(&(pool->queue_ready),&(pool->queue_lock));
		pool->cur_queue_size--;
		Thread_task *worker=pool->queue_head;
		if(worker==NULL)
		{
			cout<<" task empty"<<endl;
			exit(-1);
		}
		pool->queue_head=worker->next;
		pthread_mutex_unlock(&(pool->queue_lock));
		(*worker->func)(worker->fd,worker->efd);
		free(worker);
	}
	return NULL;
}
