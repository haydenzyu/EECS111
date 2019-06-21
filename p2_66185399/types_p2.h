#ifndef __TYPES_P2_H
#define __TYPES_P2_H

#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <sys/time.h>
#include <string>
#include <vector>
#include <unistd.h>

#define EMPTY        0
#define WOMENPRESENT 1
#define MENPRESENT   2

using namespace std;

class Person
{

	int gender; // 2: male 1: female
	std::string str_gender;
	int start;
	int end;
	long time_to_stay_ms;
	int order;

	unsigned long use_order;

public:
	Person();

	void set_gender(int data);
	int get_gender(void);

	void set_time(long data);
	int get_time();
	int ready_to_leave(void);
	
	void set_start_time(int data);
	int get_start_time();
	void set_end_time(int data);
	int get_end_time();
	int get_order();
	void set_order(int data);
};


// Class for the restroom
// You may need to add more class member variables and functions
class FittingRoom {
	int status;
	int total_people;
	int avail_rooms;
	int occupied;
	int num_women;
	int num_men;
	//int num_rooms;
	int room_cnt;
	
	// You need to define the data structure to
    // save the information of people using the restroom
	// You can probebly use Standard Template Library (STL) vector


public:
	vector<Person*> rooms;
	FittingRoom(int data);

	int get_avail_rooms();
	
	void set_total_people(int data);
	int get_total_people();

	void set_num_women(int data);
	int get_num_women();
	
	void set_num_men(int data);
	int get_num_men();
	
	void set_rooms();//create rooms
	
	// You need to use this function to print the Restroom's status
	void print_status(void);
	
	void set_status(int data);
	int get_status();


	// Call by reference
	// This is just an example. You can implement any function you need
	void woman_wants_to_enter(Person* p);
	void woman_leaves(int i);
	void man_wants_to_enter(Person* p);
	void man_leaves(int i);
	
	void inc_occupied();
	void dec_occupied();
	int get_occupied();
	void time_to_leave(int curr_time);
};


#endif
