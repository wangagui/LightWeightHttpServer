#ifndef THREAD_H
#define THREAD_H

#include<stdio.h>
#include<stdlib.h>
#include<unistd.h>
#include<sys/types.h>
#include<pthread.h>
#include<assert.h>


// task
typedef struct worker
{
	void *(*process)(void *arg);
	void *arg;
	struct worker *next;
}CThread_worker;

//thread pool structure
typedef struct
{
	pthread_mutex_t queue_lock;
	pthread_cond_t queue_ready;
	
	//all the waitting task
	CThread_worker *queue_head;

	//destroy or not
	int shutdown;
	pthread_t *threadid;

	// the maxnum that threadpool allow
	int max_thread_num;
	// the num of wating queue
	int cur_queue_size;
}CThread_pool;

void *thread_routine(void *);
int pool_destroy();
int pool_add_worker(void *(*process)(void *arg),void *arg);
void pool_init(int);
void *myprocess(void *);

#endif
