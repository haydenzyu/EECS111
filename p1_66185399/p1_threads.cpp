#include "p1_threads.h"
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <stdint.h>
#include <fstream>
#include <bits/stdc++.h>
#include <vector>

// This file includes functions that actually perform the
// computation. You can create a thread with these function
// or directly call this function


int cnt=0;
int part=0;
int num_thread=0;
using namespace std;

void sort_file(int num_threads, string path){
	vector< pair<long int, double> > vec;
	string x, y;
	long int a;
	double b;

	ifstream file(path.c_str());
	cout<<path<<endl;

	if(!file){
		cout<<"Failed to open file\n";
	}

	getline(file, y);
	//read file and store into vector
	while(!file.eof()){
		getline(file, x, ',');
		a = atol(x.c_str());
		getline(file, x, '\n');
		b = atof(x.c_str());
		vec.push_back(make_pair(a, b));
		cnt++;
	}

	num_thread = num_threads;
	pthread_t tids[num_thread];
	for(int i=0; i<num_thread; i++){
		pthread_attr_t attr;
		pthread_attr_init(&attr);
		pthread_create(&tids[i], &attr, mergesort, &vec);
	}

	for(int i=0; i<num_thread; i++){
		pthread_join(tids[i], NULL);
	}

	int elements = cnt/num_thread;
	int sections = num_thread/2;
	while(sections!=1){
		cout<<sections<<" left"<<endl;
		for(int i=0; i<sections; i++){
			merge(&vec, i*elements, (i+1)*elements-1, (i+1)*(2*elements)-1);
		}
		elements *= 2;
		sections = sections/2;
	}

	path.erase(0,6);//erase input/
	string ostr = "output/"+path;
        ofstream ofile(ostr.c_str());

        ofile<<"Rank, Student ID, Grade\n";
        for(int i=cnt; i>0; i--){
                ofile<<cnt-i+1<<", id: "<<vec[i].first<<", grade: "<<setprecision(12)<<vec[i].second<<endl;
        }
	cout<<"end of sort"<<endl;
}

void *mergesort(void *param){
	vector< pair<long int, double> >* student = (vector< pair<long int, double> >*)param;
	int elements = cnt;//temp for number of students
	int section = elements/num_thread;//students in each section
	int curr = part++;
	//cout<<"section: "<<curr<<endl;
	int min = curr*section;//start index of section
	int max = (curr+1)*section-1;//end index of section
	
	if(curr+1 == num_thread){//if last part
		max=elements;
	}
	//cout<<min<<" "<<max<<endl;

	int mid = min+(max-min)/2;
	//call sort
	if(min<max){
		mergeSort(student, min, mid);
		mergeSort(student, mid+1, max);
		merge(student, min, mid, max);
	}
}

void merge(std::vector<pair<long int, double> >* students, int l, int m, int r){ 
	int i, j, k; 
	int n1 = m - l + 1; 
	int n2 =  r - m; 

	/* create vectors */
	vector<pair<long int, double> > L; 
	vector<pair<long int, double> > R; 

	/* Copy data to pair vector L and R */
	for (i = 0; i < n1; i++){
		L.push_back(make_pair(students[0][l+i].first, students[0][l + i].second)); 
	}
	for (j = 0; j < n2; j++){
		R.push_back(make_pair(students[0][m+1+j].first, students[0][m+1+j].second)); 
	}	

	i = 0; // start of first part
	j = 0; // start of second part
	k = l; // start of merged subarray
	while (i < n1 && j < n2){ 
		if (L[i].second <= R[j].second) 
		{ 
		    students[0][k] = L[i]; 
		    i++; 
		} 
		else
		{ 
		    students[0][k] = R[j]; 
		    j++; 
		} 
		k++; 
	} 

	while (i < n1){ 
		students[0][k] = L[i]; 
		i++; 
		k++; 
	} 

	while (j < n2){ 
		students[0][k] = R[j]; 
		j++; 
		k++; 
	} 
} 
  
void mergeSort(std::vector<pair<long int, double> >* students, int l, int r){ 
	if (l < r){ 
		int m = l+(r-l)/2; 
		// Sort first and second halves 
		mergeSort(students, l, m); 
		mergeSort(students, m+1, r); 
		merge(students, l, m, r); 
	} 
} 
