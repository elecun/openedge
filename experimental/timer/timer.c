/**
 * @file    timer.cc
 * @brief   experiment for timer
 * @author  Byunghun Hwang
 */

/*
- hrtimer(High Resolution Timer)를 사용하기 위해서는 Kernel 설정시 CONFIG_HIGH_RES_TIMER를 활성화 시키고
posix timer interface를 사용하면 된다고 함. (https://kldp.org/node/124488)

- 이 실험에서는 timer를 만들어서 실행시키고 timer resolution과 precision을 확인하는데에 촛점을 맞춤.
*/

#include <stdio.h> /* for printf */
#include <stdint.h> /* for uint64 definition */
#include <stdlib.h> /* for exit() definition */
#include <time.h> /* for clock_gettime */
#include <unistd.h>

#define BILLION 1000000000L

int localpid(void) {
 static int a[9] = { 0 };
 return a[0];
}

void main(int argc, char **argv)
{

	clock_getres();

 uint64_t diff;
 struct timespec start, end;
 int i;

 /* measure monotonic time */
 clock_gettime(CLOCK_MONOTONIC, &start); /* mark start time */
 sleep(1); /* do stuff */
 clock_gettime(CLOCK_MONOTONIC, &end); /* mark the end time */

 diff = BILLION * (end.tv_sec - start.tv_sec) + end.tv_nsec - start.tv_nsec;
 printf("elapsed time = %llu nanoseconds\n", (long long unsigned int) diff);

 /* now re-do this and measure CPU time */
 /* the time spent sleeping will not count (but there is a bit of overhead */
 clock_gettime(CLOCK_PROCESS_CPUTIME_ID, &start); /* mark start time */
 sleep(1); /* do stuff */
 clock_gettime(CLOCK_PROCESS_CPUTIME_ID, &end);  /* mark the end time */

 diff = BILLION * (end.tv_sec - start.tv_sec) + end.tv_nsec - start.tv_nsec;
 printf("elapsed process CPU time = %llu nanoseconds\n", (long long unsigned int) diff);

 exit(0);
}