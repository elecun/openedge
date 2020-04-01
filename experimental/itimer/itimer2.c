#include <stdio.h>
#include <time.h>
#include <signal.h> 

struct timespec x; 
struct itimerspec ispec; 
struct itimerspec ospec; 
struct sigaction sa; 
time_t tid; 

void alrm_handler(int signo, siginfo_t *info, void *context) { 
    clock_gettime(CLOCK_REALTIME,&x); 
    printf("%ld.%09ld \n",x.tv_sec, x.tv_nsec); 
    } 
    
int main() { 
    
    x.tv_sec=0; 
    x.tv_nsec=100000000L; 
    tid=1; 
    sa.sa_sigaction = alrm_handler; 
    sa.sa_flags = SIGEV_SIGNAL; 
    sigaction(SIGALRM, &sa, NULL); 
    
    timer_create(CLOCK_REALTIME,NULL,&tid); 
    ispec.it_value.tv_sec = 0; 
    ispec.it_value.tv_nsec = 100000000L; 
    ispec.it_interval.tv_sec = 0; 
    ispec.it_interval.tv_nsec = 1; 
    timer_settime(tid, TIMER_ABSTIME, &ispec, &ospec); 
    while(1) 
        sleep(1); 
}

