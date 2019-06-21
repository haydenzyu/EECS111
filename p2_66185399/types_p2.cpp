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
#include "utils.h"




void Person::set_gender(int data) { gender = data; }

int Person::get_gender(void)      { return gender; }

void Person::set_time(long data) { time_to_stay_ms = data; }
int Person::get_time(){return time_to_stay_ms;}

void Person::set_start_time(int data){start = data;}
int Person::get_start_time(){return start;}
void Person::set_end_time(int data){end = data;}
int Person::get_end_time(){return end;}


int Person::ready_to_leave(void) {

	if (get_elasped_time(start, end) >= time_to_stay_ms) { return 1; }
	else { return 0; }
}

int Person::get_order(){return order;}
void Person::set_order(int data){order = data;}

Person::Person() {
	int gender=0;
	int start=0;
	int end=0;
	long time_to_stay_ms=0;
}


FittingRoom::FittingRoom(int data){
		status = EMPTY;
		avail_rooms = data;
		num_men=0;
		num_women=0;
		occupied=0;
		total_people=0;
		room_cnt=0;
	}
	
void FittingRoom::set_total_people(int data){total_people = data;}
int FittingRoom::get_total_people(){return total_people;}

int FittingRoom::get_avail_rooms(){return avail_rooms;}

void FittingRoom::inc_occupied(){occupied+=1;}
void FittingRoom::dec_occupied(){occupied-=1;}
int FittingRoom::get_occupied(){return occupied;}

void FittingRoom::set_num_women(int data){num_women = data;}
int FittingRoom::get_num_women(){return num_women;}
void FittingRoom::set_num_men(int data){
	num_men = data;
}
int FittingRoom::get_num_men(){
	return num_men;
}

void FittingRoom::set_rooms(){
	for(int i=0; i<avail_rooms; i++){
		rooms.push_back(new Person());
	}
}

void FittingRoom::set_status(int data){status = data;}
int FittingRoom::get_status(){return status;}

void FittingRoom::print_status(void) {
	printf("Print restroom status\n");
}

void FittingRoom::woman_wants_to_enter(Person* p) {
	for(int i=0; i<avail_rooms; i++){
		if(!rooms[i]->get_gender()){
			rooms[i]= p;
			break;
		}
	}
	num_women++;
	set_status(WOMENPRESENT);
	occupied++;
	room_cnt++;
}

void FittingRoom::woman_leaves(int i){
	Person* temp= new Person();
	rooms[i]=temp;
	occupied--;
	num_women--;
	total_people--;
	if(!num_women){
		set_status(EMPTY);
	}
}

void FittingRoom::man_wants_to_enter(Person* p) {
	for(int i=0; i<avail_rooms; i++){
		if(!rooms[i]->get_gender()){
			rooms[i]= p;
			break;
		}
	}
	num_men++;
	set_status(MENPRESENT);
	occupied++;
	room_cnt++;
}

void FittingRoom::man_leaves(int i){
	Person* temp = new Person();
	rooms[i]=temp;
	occupied--;
	num_men--;
	total_people--;
	if(!num_men){
		set_status(EMPTY);
	}
}

void FittingRoom::time_to_leave(int curr_time){
	for(int i=0; i<avail_rooms; i++){//check all rooms
		rooms[i]->set_end_time(curr_time);
		if(rooms[i]->ready_to_leave() && rooms[i]->get_time()!=0){ //Person is ready to leave
			if(rooms[i]->get_gender()==WOMENPRESENT){ //Woman ready 
				woman_leaves(i);
				cout<<"["<<curr_time<<" ms]"<<"[Fitting Room] (Woman) left the fitting room ";
				if(status==EMPTY){ //Fitting Room empty
					cout<<"Status is Changed, Status is (EMPTY): Total: 0 (Men:"
					<<num_men<<", Women: "<<num_women<<")"<<endl;
				}
				else{ //Women still in fitting room
					cout<<"Status is (Women): Total: "<<num_women<<" (Men:"
					<<num_men<<", Women: "<<num_women<<")"<<endl;
				}
				break;
			}
			else if(rooms[i]->get_gender()==MENPRESENT){//Man ready
				man_leaves(i);
				cout<<"["<<curr_time<<" ms]"<<"[Fitting Room] (man) left the fitting room ";
				if(status==EMPTY){ //Fitting Room empty
					cout<<"Status is Changed, Status is (EMPTY): Total: 0 (Men:"
					<<num_men<<", Women: "<<num_women<<")"<<endl;
				}
				else{ //Still men in fitting room
					cout<<"Status is (men): Total: "<<num_men<<" (Men:"
					<<num_men<<", Women: "<<num_women<<")"<<endl;
				}	
				break;
			}
		}
	}
}






