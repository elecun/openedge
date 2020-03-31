/* Example code for setting
 * up an interval timer in 
 * Linux. 
 * 
 * Based on existing text example
 *
 * Original Text Source:-
 * Advanced Linux Programming
 * by CodeSourcery
 *
 */

//itimer로 1msec 주기로 돌렸을때 약 4msec의 주기성을 보임.

#include <signal.h>
#include <stdio.h>
#include <string.h>
#include <sys/time.h>

void timer_handler (int signum)
{
 //static int count = 0;
 struct timeval ts;

 //count += 1;
 gettimeofday(&ts, NULL);
 printf ("%ld.%06ld\n", ts.tv_sec, ts.tv_usec);
}

int main ()
{
 struct sigaction sa;
 struct itimerval timer;

 /* Install timer_handler as the signal handler for SIGVTALRM. */
 memset (&sa, 0, sizeof (sa));
 sa.sa_handler = &timer_handler;
 sigaction (SIGVTALRM, &sa, NULL);

 /* Configure the timer to expire after ? msec... */
 timer.it_value.tv_sec = 0;
 timer.it_value.tv_usec = 5000; //5ms
 
 //time interval간격을 5000us마다, SIGALRM signal 발생됨.
 timer.it_interval.tv_sec = 0;
 timer.it_interval.tv_usec = 5000;
 /* Start a virtual timer. It counts down whenever this process is
   executing. */
 setitimer (ITIMER_VIRTUAL, &timer, NULL);

 timer_settime();

 /* Do busy work. */
 while (1);
}

