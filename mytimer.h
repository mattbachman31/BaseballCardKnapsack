#ifndef MY_TIMER
#define MY_TIMER

#include <time.h>
#include <sys/time.h>
#include <stdlib.h>
#include <stdio.h>

void start_the_clock(struct timeval* time1);

double total_time(struct timeval* t1, struct timeval* t2);

#endif
