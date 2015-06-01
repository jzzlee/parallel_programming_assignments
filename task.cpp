////////////////////////////////////
//This is the programming homework 4.5.
//author: jzzlee
//date: 2015.06.01
//
//Problem Description:
//Write a Pthreads program that implements a “task queue.” The main thread
//begins by starting a user-specified number of threads that immediately go to
//sleep in a condition wait. The main thread generates blocks of tasks to be 
//carried out by the other threads; each time it generates a new block of tasks, it
//awakens a thread with a condition signal. When a thread finishes executing
//its block of tasks, it should return to a condition wait. When the main thread
//completes generating tasks, it sets a global variable indicating that there will be
//no more tasks, and awakens all the threads with a condition broadcast. For the
//sake of explicitness, make your tasks linked list operations.
//
////////////////////////////////////////////

#include<iostream>
#include <cstdio>
#include <cstdlib>
#include <list>
#include <pthread.h>
#include <ctime>

using std::list;
struct my_struct_t
{
	pthread_mutex_t mutex;
	pthread_cond_t task;
	bool global_value;
	list<int> lst;
};
//create a my_struct_t object and initialize it		
my_struct_t data = {PTHREAD_MUTEX_INITIALIZER, PTHREAD_COND_INITIALIZER, false};

//wait the condition signal and do something
void *wait_do(void * rank)
{
	long my_rank = (long)rank;
	int local_value;
	while(1)
	{
		pthread_mutex_lock(&data.mutex);
		
		while(data.lst.empty())
		{
			//wait the task signal
			while(pthread_cond_wait(&data.task, &data.mutex) != 0);
		}
		
		if(data.lst.empty() == false)
		{
			//do something
			local_value = data.lst.front();
			data.lst.pop_front();
		}
		pthread_mutex_unlock(&data.mutex);
		//do something
		sleep(1);
		printf("%d from thread %ld\n", local_value, my_rank);
		if(data.global_value)
		{
			break;
		}
	}
	//if no more task be generated 
	if(data.global_value)
	{
		while(!data.lst.empty())
		{
			pthread_mutex_lock(&data.mutex);
			//do something
			if(data.lst.empty() == false)
			{
				local_value = data.lst.front();
				data.lst.pop_front();
			}
			pthread_mutex_unlock(&data.mutex);
			sleep(1);
			printf("%d from thread %ld\n", local_value, my_rank);
		}
	}
	return NULL;
}
const int array_size = 9;
int my_data[array_size] = {1,2,3,4,5,6,7,8,9};
int main(int argc, char *argv[])
{
	pthread_t *thread_handles;
	long thread_count = strtol(argv[1], NULL, 10);
	//at most array_size threads need to be created.
	if(thread_count > array_size)
	  thread_count = array_size;
	thread_handles = (pthread_t *)malloc(thread_count * sizeof(pthread_t));
	//create threads
	for(long thread = 0; thread != thread_count; ++thread)
	  pthread_create(&thread_handles[thread], NULL, wait_do, (void *) thread);
	//generate several tasks and send signal
	for(int i=0; i!= array_size; ++i)
	{
		pthread_mutex_lock(&data.mutex);
		data.lst.push_back(my_data[i]);
		pthread_cond_signal(&data.task);
		pthread_mutex_unlock(&data.mutex);
		printf("%d from main\n", my_data[i]);
	}
	//no more task be generated, change global_value
	pthread_mutex_lock(&data.mutex);
	data.global_value = true;
	pthread_mutex_unlock(&data.mutex);

	for(long thread = 0; thread != thread_count; ++thread)
		pthread_join(thread_handles[thread], NULL);
	pthread_mutex_destroy(&data.mutex);
	pthread_cond_destroy(&data.task);
	free(thread_handles);
	return 0;
}
