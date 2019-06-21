#ifndef __P1_PROCESS
#define __P1_PROCESS

#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <fstream>
#include <pthread.h>
#include <unistd.h>
#include <string>
#include <vector>

#include "p1_threads.h"

void get_statistics(std::string class_name[], int num_processes, int num_threads);

#endif
