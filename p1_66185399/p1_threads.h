#ifndef __P1_THREADS
#define __P1_THREADS

#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <stdint.h>
#include <fstream>
#include <bits/stdc++.h>
#include <vector>

#include "p1_process.h"

void sort_file(int num_threads, std::string path);
void merge(std::vector<std::pair<long int, double> >* students, int l, int m, int r); 
void mergeSort(std::vector<std::pair<long int, double> >* students, int l, int r);
void *mergesort(void* param);

#endif
