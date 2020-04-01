

#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <time.h>
#include <sys/time.h>
#include <pthread.h>
#include <signal.h>
#include <unistd.h>


void itimer_event_thread(void* param)
{
    struct itimerval itimer;
    sigset_t sigset;
    int sig_no = 0;
    struct timeval now;
    unsigned int time_gap_sec = 0;
    unsigned int time_gap_usec = 0;

    printf("%s() start!!\n", __func__);

    //block alarm signal, will be waited on explicitly
    sigemptyset(&sigset);
    sigaddset(&sigset, SIGALRM);
    sigprocmask(SIG_BLOCK, &sigset, NULL);

    //unblock
    pthread_sigmask(SIG_UNBLOCK, &sigset, NULL);

    //set up periodic interrupt timer * start
    itimer.it_value.tv_sec = 0;
    itimer.it_value.tv_usec = 10*1000;
    itimer.it_interval = itimer.it_value;

    if(setitimer(ITIMER_REAL, &itimer, NULL)!=0){
        printf("could not start interval timer : %s", strerror(errno));
        return;
    }

    //wait signal
    while(1){
        if(sigwait(&sigset, &sig_no)!=0){
            printf("failed to wait for next clock tick\n");
        }

        switch(sig_no){
            case SIGALRM:
            gettimeofday(&now, NULL);
            printf("now= %ld.%06ld, gap=%ld.%06ld\n", now.tv_sec, now.tv_usec, now.tv_sec-time_gap_sec, now.tv_usec-time_gap_usec);
            time_gap_sec = now.tv_sec;
            time_gap_usec = now.tv_usec;
            break;

            default:
            printf("unexpected signal %d\n", sig_no);
        }
    }
    return;
}

int main(){
    pthread_t thread;
    struct itimerval itimer;
    sigset_t sigset;
    int sig_no=0;
    int rc;

    signal(SIGALRM, SIG_IGN);

    //block alarm signal
    sigemptyset(&sigset);
    sigaddset(&sigset, SIGALRM);
    sigprocmask(SIG_BLOCK, &sigset, NULL);

    //block sigset main thread
    pthread_sigmask(SIG_BLOCK, &sigset, NULL);

    rc = pthread_create(&thread, NULL, (void*)itimer_event_thread, NULL);
    if(rc)
        printf("itimer_event_thread : %s\n", strerror(errno));
    else
        (void) pthread_detach(thread);

    while(1)
        usleep(1000);

    return 0;
    

}