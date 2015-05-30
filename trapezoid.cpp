////////////////////////////////////
//This is the programming homework 4.3.
//author: jzzlee
//date: 2015.05.30
//
//Problem Description:
//Write a Pthreads program that implements the trapezoidal rule.
//Use mutexes to enforce mutual exclusion in the critical section.
//
////////////////////////////////////////////


#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <random>
#include <ctime>

int thread_count;
pthread_mutex_t mutex;
double total_area, left = 0.0, right = 3.0;
unsigned long long total_trap_count;

void *cal_trap_area(void *rank);

int main(int argc, char * argv[])
{
	long thread;
	pthread_t *thread_handles;

	pthread_mutex_init(&mutex, NULL);

	scanf("%lld", &total_trap_count);

	thread_count = strtol(argv[1], NULL, 10);
	thread_handles = (pthread_t *) malloc(thread_count * sizeof(pthread_t));
	
	for(thread = 0; thread != thread_count; ++thread)
	  pthread_create(&thread_handles[thread], NULL, cal_trap_area, (void*) thread);
	
	for(thread = 0; thread < thread_count; ++thread)
	  pthread_join(thread_handles[thread], NULL);
	
	free(thread_handles);

	printf("%f\n", total_area);
	return 0;
}

//the function
double func(double a)
{
	return a*a;
}
// the trapezoid area from local_a to local_b
double Trap(double local_a, double local_b, unsigned long long trap_count, double base_len)
{
	double x, estimate = (func(local_a) + func(local_b))/2;
	for(unsigned long long i = 1; i <= trap_count; ++i)
	{
		x = local_a + i*base_len;
		estimate += func(x);
	}
	estimate *= base_len;
	return estimate;
}
void *cal_trap_area(void *rank)
{
	long my_rank = (long) rank;

	unsigned long long trap_count = total_trap_count / thread_count;
	double base_len = (right - left) / total_trap_count;

	double local_a = left + my_rank*trap_count*base_len;
	double local_b = local_a + trap_count*base_len;

	double local_area = Trap(local_a, local_b, trap_count, base_len);
	
	pthread_mutex_lock(&mutex);
	total_area += local_area;
	pthread_mutex_unlock(&mutex);
	return NULL;
}
