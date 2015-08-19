#include "mytimer.h"

void start_the_clock(struct timeval* time1){
	gettimeofday(time1, NULL);
}

double total_time(struct timeval* t1, struct timeval* t2){
	gettimeofday(t2, NULL);
	double ret = ((t2->tv_sec * 1000000 + t2->tv_usec) - (t1->tv_sec * 1000000 + t1->tv_usec));
	ret /= 1000000;
	return ret;
}
