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
    x.tv_nsec=10*1000*1000; 
    tid=1; 
    sa.sa_sigaction = alrm_handler; 
    sa.sa_flags = SIGEV_SIGNAL; 
    sigaction(SIGALRM, &sa, NULL); 
    
    timer_create(CLOCK_REALTIME,NULL,&tid); 
    ispec.it_value.tv_sec = 0; 
    ispec.it_value.tv_nsec = 1*1000*1000; 
    ispec.it_interval.tv_sec = 0; 
    ispec.it_interval.tv_nsec = 1*1000*1000; 
    timer_settime(tid, TIMER_ABSTIME, &ispec, &ospec); 
    while(1) 
        sleep(1); 
}

