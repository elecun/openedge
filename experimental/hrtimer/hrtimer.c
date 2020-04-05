


#include <linux/hrtimer.h>
#include <linux/sched.h>

/****************************************************************************/
/* Timer variables block                                                    */
/****************************************************************************/
static enum hrtimer_restart function_timer(struct hrtimer *);
static struct hrtimer htimer;
static ktime_t kt_periode;

kt_periode = ktime_set(0, 100); //seconds,nanoseconds
hrtimer_init (& htimer, CLOCK_REALTIME, HRTIMER_MODE_REL);
htimer.function = function_timer;
hrtimer_start(& htimer, kt_periode, HRTIMER_MODE_REL);