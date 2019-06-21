#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <sys/time.h>
#include <string>
#include <vector>
#include <unistd.h>
#include <pthread.h>
#include "utils.h"


int get_elasped_time(int start, int end)
{

	int mtime = end - start;

	return mtime;
}



