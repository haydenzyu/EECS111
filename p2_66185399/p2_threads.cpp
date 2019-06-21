#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <sys/time.h>
#include <string>
#include <vector>
#include <unistd.h>
#include <pthread.h>
#include <queue>
#include "p2_threads.h"
#include "utils.h"
#include "types_p2.h"

extern pthread_cond_t QueueCond;
extern pthread_mutex_t mutex;

queue<Person*> men_waitingQ;
queue<Person*> women_waitingQ;
int t_men=0;
int t_women=0;
int time_cnt=0;
int t_order=0;
int no_more=0;

void *line(void *param){
	printf(" [Line Thread] Start\n");
	FittingRoom* t_room = (FittingRoom*) param;
	int status;
	int rand_gender;
	long rand_time;
	int rand_time_wait;
	int total_people1 = t_room->get_total_people();
	srand(time(0));
	
	while(t_men+t_women<total_people1){
		rand_gender = rand()%2+1; //random 1(woman) or 2(man)
		if(t_women>=total_people1/2)//if either gender has enough people
			rand_gender = MENPRESENT;
		else if(t_men>=total_people1/2)
			rand_gender = WOMENPRESENT;
		
		rand_time = rand()%7+3; //random 3ms to 10ms
		rand_time_wait = rand()%5+1; //rand 1ms to 5ms
		
		//create person
		Person* z = new Person();
		z->set_gender(rand_gender);
		z->set_time(rand_time);
		
		
		if((z->get_gender()==MENPRESENT && t_men <= total_people1/2)){//add men to waiting
			pthread_mutex_lock(&mutex);
			z->set_order(t_order);
			t_men++; //number of men increment by 1
			men_waitingQ.push(z); //add man to queue
			cout<<"["<<time_cnt<<" ms]"<<"[Input] A person (Man) goes into the queue"<<endl;
			t_order++;
			pthread_cond_broadcast(&QueueCond);
			
		}
		else if(z->get_gender()==WOMENPRESENT && t_women <= total_people1/2){//add women to waiting
			pthread_mutex_lock(&mutex);
			z->set_order(t_order);
			t_women++; //number of women increment by 1
			women_waitingQ.push(z);//add woman to queue
			cout<<"["<<time_cnt<<" ms]"<<"[Input] A person (Woman) goes into the queue"<<endl;
			t_order++;
			pthread_cond_broadcast(&QueueCond);
		}
		else{
			cout<<"No more people to put in the queue\n";
			pthread_mutex_unlock(&mutex);
		}
		usleep(MSEC(rand_time_wait)); //wait a random time
		time_cnt+=rand_time_wait; //time counter increases
		t_room->time_to_leave(time_cnt);
		pthread_mutex_unlock(&mutex);
		usleep(MSEC(1));
	}
	no_more=1;
	pthread_cond_broadcast(&QueueCond);
	pthread_mutex_unlock(&mutex);
	
}
		
		
		
void *assign(void *param){
	printf(" [Assign Thread] Start\n");
	FittingRoom* t_room = (FittingRoom*) param;
	while(t_room->get_total_people()){
		if(!(men_waitingQ.empty())|| !(women_waitingQ.empty())){//if the waiting queue isn't empty
			//assign thread locks
			pthread_mutex_lock(&mutex);
			if(t_room->get_occupied()<t_room->get_avail_rooms()){//checks if fitting room is full
				if(t_room->get_status()==WOMENPRESENT && !women_waitingQ.empty()){//check if women are in fitting room
					Person* w = women_waitingQ.front();
					w->set_start_time(time_cnt);//start time for person
					
					cout<<"["<<time_cnt<<" ms]"<<"[Queue] Send (Woman) into the fitting room (Stay "<<w->get_time()<<" ms), Status: Total: "
					<<t_room->get_num_women()<<"(Men: "<<t_room->get_num_men()<<", Women: "<<t_room->get_num_women()<<")"<<endl;
					
					t_room->woman_wants_to_enter(w); //put woman in the fitting with start time
					women_waitingQ.pop();
					
					cout<<"["<<time_cnt<<" ms]"<<"[Fitting Room] (Woman) goes into the fitting room, State is (WomenPresent): "<<"Total: "
					<<t_room->get_num_women()<<"(Men: "<<t_room->get_num_men()<<", Women: "<<t_room->get_num_women()<<")"<<endl;
				}
				else if(t_room->get_status()==MENPRESENT && !men_waitingQ.empty()){//check if men are in fitting room
					Person* m = men_waitingQ.front();
					m->set_start_time(time_cnt);//start time for person
					
					cout<<"["<<time_cnt<<" ms]"<<"[Queue] Send (man) into the fitting room (Stay "<<m->get_time()<<" ms), Status: Total: "
					<<t_room->get_num_men()<<"(Men: "<<t_room->get_num_men()<<", Women: "<<t_room->get_num_women()<<")"<<endl;
					
					t_room->man_wants_to_enter(m); //put man in the fitting with start time
					men_waitingQ.pop();
					
					cout<<"["<<time_cnt<<" ms]"<<"[Fitting Room] (man) goes into the fitting room, State is (MenPresent): "<<"Total: "
					<<t_room->get_num_men()<<"(Men: "<<t_room->get_num_men()<<", Women: "<<t_room->get_num_women()<<")"<<endl;
				}
				else if(t_room->get_status()==EMPTY){
					if(!men_waitingQ.empty() && !women_waitingQ.empty()){ //when both men and women queue have people
							Person* m = men_waitingQ.front();
							Person* w = women_waitingQ.front();
						if(w->get_order() < m->get_order()){ //if women is before men
							
							w->set_start_time(time_cnt);//start time for person
							
							cout<<"["<<time_cnt<<" ms]"<<"[Queue] Send (Woman) into the fitting room (Stay "<<w->get_time()<<" ms), Status: Total: 0"
							<<"(Men: "<<t_room->get_num_men()<<", Women: "<<t_room->get_num_women()<<")"<<endl;
							
							t_room->woman_wants_to_enter(w); //put woman in the fitting with start time
							women_waitingQ.pop();
							
							cout<<"["<<time_cnt<<" ms]"<<"[Fitting Room] (Woman) goes into the fitting room, State is (WomenPresent): "<<"Total: "
							<<t_room->get_num_women()<<"(Men: "<<t_room->get_num_men()<<", Women: "<<t_room->get_num_women()<<")"<<endl;
						}
						else if(w->get_order() > m->get_order()){//if men is before women
							
							m->set_start_time(time_cnt);//start time for person
							
							cout<<"["<<time_cnt<<" ms]"<<"[Queue] Send (man) into the fitting room (Stay "<<m->get_time()<<" ms), Status: Total: 0"
							<<"(Men: "<<t_room->get_num_men()<<", Women: "<<t_room->get_num_women()<<")"<<endl;
							
							t_room->man_wants_to_enter(m); //put man in the fitting with start time
							men_waitingQ.pop();
							
							cout<<"["<<time_cnt<<" ms]"<<"[Fitting Room] (man) goes into the fitting room, State is (MenPresent): "<<"Total: "
							<<t_room->get_num_men()<<"(Men: "<<t_room->get_num_men()<<", Women: "<<t_room->get_num_women()<<")"<<endl;
						}
					}
					else if(men_waitingQ.empty()){//men queue is empty send woman
						Person* w = women_waitingQ.front();
						//usleep(MSEC(1));
						//time_cnt+=1;
						w->set_start_time(time_cnt);//start time for person
						
						cout<<"["<<time_cnt<<" ms]"<<"[Queue] Send (Woman) into the fitting room (Stay "<<w->get_time()<<" ms), Status: Total: 0"
						<<"(Men: "<<t_room->get_num_men()<<", Women: "<<t_room->get_num_women()<<")"<<endl;
						
						t_room->woman_wants_to_enter(w); //put woman in the fitting with start time
						women_waitingQ.pop();
						
						cout<<"["<<time_cnt<<" ms]"<<"[Fitting Room] (Woman) goes into the fitting room, State is (WomenPresent): "<<"Total: "
						<<t_room->get_num_women()<<"(Men: "<<t_room->get_num_men()<<", Women: "<<t_room->get_num_women()<<")"<<endl;
					}
					else if(women_waitingQ.empty()){//women queue is empty send man
						Person* m = men_waitingQ.front();
				
						m->set_start_time(time_cnt);//start time for person
						
						cout<<"["<<time_cnt<<" ms]"<<"[Queue] Send (man) into the fitting room (Stay "<<m->get_time()<<" ms), Status: Total: 0"
						<<"(Men: "<<t_room->get_num_men()<<", Women: "<<t_room->get_num_women()<<")"<<endl;
						
						t_room->man_wants_to_enter(m); //put man in the fitting with start time
						men_waitingQ.pop();
						
						cout<<"["<<time_cnt<<" ms]"<<"[Fitting Room] (man) goes into the fitting room, State is (MenPresent): "<<"Total: "
						<<t_room->get_num_men()<<"(Men: "<<t_room->get_num_men()<<", Women: "<<t_room->get_num_women()<<")"<<endl;
					}
				}
			}	
			else{
				cout<<"Fitting Room is full\n"; 
				t_room->time_to_leave(time_cnt);
			}
			t_room->time_to_leave(time_cnt);
		}
		else if(men_waitingQ.empty()&& women_waitingQ.empty() && !no_more){
			cout<<"Waiting Queue is empty\n";
			pthread_cond_wait(&QueueCond, &mutex);//waiting for queue signal
		}
		else{
			t_room->time_to_leave(time_cnt);
		}
		t_room->time_to_leave(time_cnt);
		//Assign thread unlocks
		pthread_mutex_unlock(&mutex);
		usleep(MSEC(1));
		time_cnt+=1;
	}
	cout<<"Everyone is fitted"<<endl;
}
