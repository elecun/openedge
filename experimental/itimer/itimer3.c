

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
    sigset_t sigset; //block 될 시그널set
    int sig_no = 0;
    struct timeval now;
    unsigned int time_gap_sec = 0;
    unsigned int time_gap_usec = 0;

    printf("%s() start!!\n", __func__);

    //block alarm signal, will be waited on explicitly
    sigemptyset(&sigset);  //시그널set을 초기화하고
    sigaddset(&sigset, SIGALRM); //sigalrm을 sigset에 추가
    sigprocmask(SIG_BLOCK, &sigset, NULL); //sigset에 있는 signal들을 block한다.

    //unblock
    pthread_sigmask(SIG_UNBLOCK, &sigset, NULL); //thread내의 시그널 처리 : sigset에서 설정한 mask들이 현재 thread의 signal mask에서 제거된다. 즉,  sigset에 설정된 signal을 이 thread에서는 허용하겠다는 것.

    //set up periodic interrupt timer * start
    itimer.it_value.tv_sec = 0;
    itimer.it_value.tv_usec = 10*1000;
    itimer.it_interval = itimer.it_value;

//setitimer에서 itimer_real로 설정하면, 타이머가 만료되면 sigarlm 시그널을 프로세스에 전달한다.
//만약에 setitimer에서 itimer_virutal로 설정하면, 타이머가 만료되면 sigvtalrm 시그널을 프로세스에 전달한다.
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