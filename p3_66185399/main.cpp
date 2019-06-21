//////////////////////////////////
/* Name: Hayden Yu */
/* ID: 66185399 */
/* Date: 6/7/19 */
//////////////////////////////////
#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <sys/time.h>
#include <string>
#include <vector>
#include <unistd.h>
#include <pthread.h>
#include <algorithm>
#include "types_p3.h"
#include "p3_threads.h"
#include "utils.h"

pthread_cond_t  cond[4];
pthread_cond_t a_task_is_done;
ThreadCtrlBlk   tcb[4];
std::vector<int> readyQue;

int num_of_alive_tasks=4;
int occupied = 0; //thread availability


pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t taskDoneMutex = PTHREAD_MUTEX_INITIALIZER;

struct timeval t_global_start;
int global_work = 0; //main process working or not

void fifo_schedule(void);
void edf_schedule(void);
void rm_schedule(void);
int search_ed(void);
int search_period(void);

using namespace std;

int main(int argc, char** argv)
{
	if(argc !=2 || atoi(argv[1]) < 0 || atoi(argv[1]) > 2)
	{
		std::cout << "[ERROR] Expecting 1 argument, but got " << argc-1 << std::endl;
		std::cout<< "[USAGE] p3_exec <0, 1, or 2>" << std::endl;
		return 0;
	}
	int schedule = atoi(argv[1]);



	pthread_t       tid[4];
	int             status = 0;
	int             id = 0;
	long            stamp = 0;
	int             mode = 0;


	
	// This is to set the global start time
	gettimeofday(&t_global_start, NULL);	



	tcb[0].id = 0;
	tcb[0].task_time = 200; //time for thread to run
	tcb[0].period = 1000; //period given to the thread
	tcb[0].deadline = 1000; //deadline for the thread

	tcb[1].id = 1;
	tcb[1].task_time = 500;
	tcb[1].period = 2000;
	tcb[1].deadline = 2000;

	tcb[2].id = 2;
	tcb[2].task_time = 1000;
	tcb[2].period = 4000;
	tcb[2].deadline = 4000;

	tcb[3].id = 3;
	tcb[3].task_time = 1000;
	tcb[3].period = 6000;
	tcb[3].deadline = 6000;

	//initializing conditions
	for (int i=0; i<4; i++) {
		pthread_cond_init (&(cond[i]), NULL);
	}
	pthread_cond_init (&a_task_is_done, NULL); 
	

	global_work = 1;
	printf("[Main] Create worker threads\n");
	for (int i=0; i<4; i++) {
		if(pthread_create(&(tid[i]), NULL, threadfunc, &(tcb[i]))) {
			fprintf(stderr, "Error creating thread\n");		
		}
	}

	// Wait until the thread is in place
	usleep(MSEC(1000));
	
	
	// This is to reset the global time and skip the initial wait
	gettimeofday(&t_global_start, NULL);	
	
	
	int sleep = 0;
	for (int i=0; i<240; i++) {	
		stamp = get_time_stamp();

		/////////////////////////////////////////////////////////
		// Implement different scheduling algorithms
        // Select different schedule based on mode value
		switch(schedule)
		{
			case 0:
				fifo_schedule();
				break;
			case 1:
				edf_schedule();
				break;
			case 2:
				rm_schedule();
				break;
		}
		/////////////////////////////////////////////////////////
		// Wait for a fraction of 100ms or until a task is done.

		stamp = get_time_stamp();
		sleep = 100 - (stamp%100);
		if (num_of_alive_tasks>0)
			timed_wait_for_task_complition(sleep);
		else{
			printf("All the tasks missed the deadline");
			break;
		}
	}


	printf("[Main] It's time to finish the thread\n");



	printf("[Main] Locks\n");
	pthread_mutex_lock(&mutex); //critical section starts
	global_work = 0;
	printf("[Main] Unlocks\n");

	
	// make sure all the process are in the ready queue
	usleep(MSEC(3000));
	while (readyQue.size()>0) {
		pthread_cond_signal(&(cond[readyQue[0]]));
		readyQue.erase(readyQue.begin()+0);

	}


	pthread_mutex_unlock(&mutex); //critical section ends



	/* wait for the second thread to finish */
	for (int i=0; i<4; i++) {
		if(pthread_join(tid[i], NULL)) {
			fprintf(stderr, "Error joining thread\n");	
		}
	}


	return 0;
}


void fifo_schedule(void)
{	
	int next_id;
	pthread_mutex_lock(&mutex); //lock
	if(readyQue.empty()) //if there's no threads in the queue
		pthread_mutex_unlock(&mutex);//unlock
	else{
		next_id = readyQue.front(); //first in queue
		pthread_cond_signal(&(cond[next_id])); //schedule the thread
		pthread_cond_wait(&a_task_is_done, &mutex); //wait for it to be done
		readyQue.erase(readyQue.begin()); //dequeue the first 
		pthread_mutex_unlock(&mutex);//unlock
	}	
	pthread_mutex_unlock(&mutex);//unlock
}


void edf_schedule(void)
{
	int next_id;
	pthread_mutex_lock(&mutex);//lock
	if(readyQue.empty())//if queue is empty
		pthread_mutex_unlock(&mutex); //unlock
	else if(readyQue.size()>1){//if the queue has more than 1 thread
		next_id = search_ed(); //find the thread with earliest deadline
		pthread_cond_signal(&(cond[next_id]));//schedule the thread
		pthread_cond_wait(&a_task_is_done, &mutex); //wait for it to be done
		readyQue.erase(remove(readyQue.begin(), readyQue.end(), next_id), readyQue.end());//delete that thread
		pthread_mutex_unlock(&mutex);//unlock
	}
	else{//only one thread in the queue, same as FIFO
		next_id = readyQue.front();
		pthread_cond_signal(&(cond[next_id]));
		pthread_cond_wait(&a_task_is_done, &mutex);
		readyQue.erase(readyQue.begin());
		pthread_mutex_unlock(&mutex);
	}
	pthread_mutex_unlock(&mutex);//unlock
}

void rm_schedule(void)
{
	int next_id;
	pthread_mutex_lock(&mutex);//lock
	if(readyQue.empty())//if queue is empty
		pthread_mutex_unlock(&mutex); //unlock
	else if(readyQue.size()>1){//if the queue has more than 1 thread
		next_id = search_period(); //find the thread with shortest period
		pthread_cond_signal(&(cond[next_id]));//schedule the thread
		pthread_cond_wait(&a_task_is_done, &mutex); //wait for it to be done
		readyQue.erase(remove(readyQue.begin(), readyQue.end(), next_id), readyQue.end());//delete that thread
		pthread_mutex_unlock(&mutex);//unlock
	}
	else{//only one thread in the queue, same as FIFO
		next_id = readyQue.front();
		pthread_cond_signal(&(cond[next_id]));
		pthread_cond_wait(&a_task_is_done, &mutex);
		readyQue.erase(readyQue.begin());
		pthread_mutex_unlock(&mutex);
	}
	pthread_mutex_unlock(&mutex);//unlock
}

int search_ed(void){//finds the thread id with earliest deadline
	int earliest_id, temp_id;
	earliest_id = readyQue[0];
	for(int i=1; i<readyQue.size(); i++){//compare with other deadlines in the queue
		temp_id = readyQue[i];
		earliest_id = (tcb[earliest_id].deadline<tcb[temp_id].deadline)? earliest_id:temp_id;
	}
	return earliest_id;
}

int search_period(void){//finds the thread id with shortest period
	int shortest_id, temp_id;
	shortest_id = readyQue[0];
	for(int i=1; i<readyQue.size(); i++){//compare with other periods in the queue
		temp_id = readyQue[i];
		shortest_id = (tcb[shortest_id].period<tcb[temp_id].period)? shortest_id:temp_id;
	}
	return shortest_id;
}


