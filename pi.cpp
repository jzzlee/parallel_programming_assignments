////////////////////////////////////
//This is the programming homework 4.2.
//author: jzzlee
//date: 2015.05.29
//
//Problem Description:
//Write a Pthreads program that uses a Monte Carlo method to estimate π.
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
long long whole_number_in_circle = 0, whole_number_of_tosses;

std::default_random_engine generator(time(NULL));
std::uniform_real_distribution<double> dis(-1,1);

void *cal_pi(void *rank);

int main(int argc, char * argv[])
{
	long thread;
	pthread_t *thread_handles;

	pthread_mutex_init(&mutex, NULL);

	scanf("%lld", &whole_number_of_tosses);

	thread_count = strtol(argv[1], NULL, 10);
	thread_handles = (pthread_t *) malloc(thread_count * sizeof(pthread_t));
	
	for(thread = 0; thread != thread_count; ++thread)
	  pthread_create(&thread_handles[thread], NULL, cal_pi, (void*) thread);
	
	for(thread = 0; thread < thread_count; ++thread)
	  pthread_join(thread_handles[thread], NULL);
	
	free(thread_handles);
	
	double my_pi = 4*whole_number_in_circle / static_cast<double>(whole_number_of_tosses);
	printf("%f\n", my_pi);
	return 0;
}

void *cal_pi(void *rank)
{
	long long number_of_tosses = whole_number_of_tosses / thread_count;
	long long number_in_circle = 0;
	for(long long toss = 0; toss < number_of_tosses; ++toss)
	{
		double x = dis(generator);
		double y = dis(generator);
		double distance_square = x*x + y*y;
		if(distance_square <= 1)
			++number_in_circle;
	}
	
	pthread_mutex_lock(&mutex);
	whole_number_in_circle += number_in_circle;
	pthread_mutex_unlock(&mutex);
	return NULL;
}
