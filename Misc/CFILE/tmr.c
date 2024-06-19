/* Create a timer function and notify via signal
 *
 * */

#include <stdio.h>
#include <time.h>
#include <stdlib.h>
#include <signal.h>
#include <string.h>
#include <errno.h>
//#include <clock.h>

static struct itimerspec Timeout;
/* struct itimerspec {
 *   struct timespec it_interval; //interval for periodic timer
 *   struct timespec it_value;    //initial expiration
 * };
 * struct timespec {
 *   time_t tv_sec;  //seconds
 *   long   tv_nsec; //Nanoseconds
 * }
 * */
static struct sigevent sigevent={0};
/* union sigval { int sival_int; //integer value void *sival_ptr;//pointer value}; //data passed with Notification 
 * struct sigevent {
 *   int sigev_notify; //Notification method
 *   int sigev_signo;  //Notification signal
 *   union sigval sigev_value; //Notification data
 *   void (*sigev_notify_function)(union sigval) // Function used for thread notification (SIGEV_THREAD)
 *   void *sigev_notify_attributes; //Attributes for notification thread (SIGEV_THREAD)
 *   pid_t sigev_notify_thread_id; //Id of thread to signal (SIGEV_THREAD_ID)
 * };
 *
 * */
static timer_t TimerID;

void timeoutnotify(union sigval id) {
  printf("%s: Timeout reached timer ID 0x%lx\nExiting\n", __FUNCTION__, (long)*((timer_t *)id.sival_ptr));
  exit(EXIT_SUCCESS);
}

void gettimeout(const int sec) {
  memset(&Timeout, 0, sizeof(Timeout));
  Timeout.it_value.tv_sec = sec;
  Timeout.it_value.tv_nsec = 0;
}

#define FAILCLK "Failed to get time via clock function"
#define ONESECTOMICROSEC 1000000UL
#define ONESECTOMILLISEC 1000UL
//get timer via free running counter
unsigned long count32() {
  unsigned long uc;
  struct timespec ts;

  if (-1 == clock_gettime(CLOCK_MONOTONIC, &ts)) {
    perror(FAILCLK);
    return errno;
  }
  
  uc = ts.tv_sec * ONESECTOMICROSEC;
  uc += ts.tv_nsec / ONESECTOMILLISEC;
  return uc;
}

void GetTimer32(unsigned long *ul) {
  do {
    *ul = count32(); //get time in milliseconds
     printf("%s: %lu\n", __FUNCTION__, *ul);
  } while (*ul == 0UL);
}

void SetTimer32(unsigned long *ul, unsigned long c) {
  do {
  *ul = count32() + c;
   printf("%s: %lu\n", __FUNCTION__, *ul);
  } while (*ul == 0UL);
}

long tm;
int main() {

  GetTimer32(&tm);
  SetTimer32(&tm, 1000);
  //set time
  gettimeout(1);
  //set event
  printf("sig %u\n", SIGEV_THREAD);
  sigevent.sigev_notify = SIGEV_THREAD;
  sigevent.sigev_value.sival_ptr = &TimerID;
  sigevent.sigev_notify_function = (void *)timeoutnotify;
  sigevent.sigev_notify_attributes = NULL;
  /* CLOCK_MONOTONIC
   * a non-settable monotonically increasing clock which measures time from unspecified point
   * in the past that does not change after the system startup
   * CLOCK_REALTIME
   * a system wide real-time clock
   * */
  int ret = timer_create(CLOCK_MONOTONIC, &sigevent, &TimerID);
  if (ret == -1) {
    printf("Failed to createtimer **%s**\n", strerror(errno));
    return -1;
  }

  if (timer_settime(TimerID, 0, &Timeout, NULL) == -1) {
    printf("failed to start timer **%s**\n", strerror(errno));
    return -1;
  }
  GetTimer32(&tm);
  for(;;);
  return 0;
}
