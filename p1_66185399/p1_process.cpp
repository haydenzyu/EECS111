#include "p1_process.h"
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <stdint.h>
#include <fstream>
#include <bits/stdc++.h>
#include <vector>
#include <sys/wait.h>

using namespace std;

void get_statistics(std::string class_name[], int num_processes, int num_threads) {

	// You need to create multiple processes here (each process
	// handles at least one file.)
	
	
	// Each process should use the sort function which you have defined  		
	// in the p1_threads.cpp for multithread sorting of the data. 
	
	// Your code should be able to get the statistics and generate
	// the required outputs for any number of child processes and threads.

	string paths[5];
	for(int i=0; i<num_processes; i++){
		paths[i] = "input/" + class_name[i] + ".csv";
	}	

	for(int i=0; i<num_processes; i++){
		if(fork()==0){
			cout<<"Child process "<<getpid()<<" created."<<endl;	
			for(int j=0; j<5; j++){
				sort_file(num_threads, paths[j]);
			}
			cout<<"Child process terminated"<<endl;
			exit(0);
		}
		else
			continue;
	}
	for(int i=0; i<num_processes; i++){
		wait(NULL);
	}
	//end of processes
}

