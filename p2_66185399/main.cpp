#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <sys/time.h>
#include <string>
#include <vector>
#include <unistd.h>
#include <pthread.h>
#include <queue>
#include "types_p2.h"
#include "p2_threads.h"
#include "utils.h"

using namespace std;

pthread_cond_t  QueueCond  = PTHREAD_COND_INITIALIZER;
pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
struct timeval t_global_start;


int main(int argc, char** argv)
{
	if(argc < 3){
		cout<<"[ERROR] Expecting 3 arguments, but got "<<argc<<endl;
		cout<<"[USAGE] p2_exec <each_gender_cnt> <number_of_stalls>"<<endl;
		exit(0);
	}
	
	if(!atoi(argv[1]) || !atoi(argv[2])){
		cout<<"[ERROR] Expecting input <each_gender_cnt> and <number_of_stalls> larger than 0"<<endl;
		exit(0);
	}
	
	pthread_t       QueueThread, AssignThread;
	
	int num_rooms = atoi(argv[2]);
	int gender_cnt = atoi(argv[1]);
	int num_people = gender_cnt*2;
	
	FittingRoom* r = new FittingRoom(num_rooms);
	r->set_total_people(num_people);
	r->set_rooms();
	int retQueue = pthread_create(&QueueThread, NULL, line, r); //create QueueThread
	int retAssign = pthread_create(&AssignThread, NULL, assign, r); //create RoomThread

	if(!retQueue)
		pthread_join(QueueThread, NULL);
	if(!retAssign)
		pthread_join(AssignThread, NULL);
	return 0;


}

