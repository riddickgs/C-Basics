/* Note: This application requires root privilege to run
 * to enable privilege run below command for the binary
 * "sudo setcap 'cap_sys_nice=eip' <application>"
 * author: Riddick
 * Last run: segmentation fault
 * pthread APIs with lib -pthread (pthread_threading_attr.h)
 * to find predefined macros from c file do below
 * cpp -dM <file> or cpp -include <file> -dM /dev/null where cpp is C preprocessor
 * int pthread_create(pthread_t *t, pthread_attr_t *attr, void *(*routine)(void *), void *args);
 * int pthread_join(pthread_t t, void **retval);
 * int pthread_attr_init(pthread_attr_t *attr);
 * int pthread_attr_destroy(pthread_attr_t *attr);
 * int pthread_attr_getdetachstate(const pthread_attr_t *attr, int *state); PTHREAD_CREATE_DETACHED|PTHREAD_CREATE_JOINABLE
 * int pthread_attr_setdetachstate(pthread_attr_t *attr, int state); PTHREAD_CREATE_DETACHED|PTHREAD_CREATE_JOINABLE
 * int pthread_attr_getschedparam(const pthread_attr_t *attr, struct sched_param *sp);
 * int pthread_attr_setschedparam(pthread_attr_t *attr, const struct sched_param *sp);
 * int pthread_attr_getschedpolicy(const pthread_attr_t *attr, int *policy); (SCHED_FIFO|SCHED_RR|SCHED_OTHER)
 * int pthread_attr_setschedpolicy(pthread_attr_t *attr, int policy); (SCHED_FIFO|SCHED_RR|SCHED_OTHER)
 * int pthread_attr_getinheritsched(const pthread_attr_t *attr, int *policy); PTHREAD_INHERIT_SCHED|PTHREAD_EXPILICT_SCHED
 * int pthread_attr_setinheritsched(pthread_attr_t *attr, int policy);
 * int pthread_attr_getscope(const pthread_attr_t *attr, int *scope); PTHREAD_SCOPE_SYSTEM|PTHREAD_SCOPE_PROCESS
 * int pthread_attr_setscope(pthread_attr_t *attr, int scope); PTHREAD_SCOPE_SYSTEM|PTHREAD_SCOPE_PROCESS
 * int pthread_attr_getstackaddr(const pthread_attr_t *attr, void **stackaddr);
 * int pthread_attr_setstackaddr(pthread_attr_t *attr, void *stackaddr);
 * int pthread_attr_getstacksize(const pthread_attr_t *attr, size_t *stacksize);
 * int pthread_attr_setstacksize(pthread_attr_t *attr, size_t stacksize);
 * int pthread_attr_getguardsize(const pthread_attr_t *attr, size_t *guardsize);
 * int pthread_attr_setguardsize(pthread_attr_t *attr, size_t guardsize);
 *
 * pthread_join
 * */
#define _GNU_SOURCE

#include <pthread.h>
#include <limits.h>
#include "newtool.h"
#include "dlink.h"

//static DLINK *pTaskMgmtHead = NULL;
TASK_MGMT *pgTM = NULL;
void clocksleep(useconds_t t)
{
   usleep(t * CLOCK_PER_SEC);
}

static void *initTask(void *arg) {
   clocksleep(120 * CLOCK_PER_SEC);
   pthread_exit(NULL);
}

/*here all cleanup after thread exited*/
void MainCleanUp()
{
  printf("Cleaning up\nGoing to exit\n");
}

int getNewThread(unsigned long *taskId, char *bName, void *(*pfnTask)(void *), unsigned long ulPrio, unsigned int stackSz, void *pfnArgs)
{
  char name[NAMELEN+1] = {0};
  int rv;
  int stackSize = (stackSz * PTHREAD_STACK_MIN);
  int minPrio;
  int maxPrio;
  struct sched_param sp;
  TASK_MGMT *pTM = NULL;

  memcpy(name, DEFBASENAME, strlen(DEFBASENAME));
  strncat(name, bName, NAMELEN);
  int sizeReq = blocksizeof(TASK_MGMT)+blocksize(stackSz); // Task mgnt + data area
  void *pv = malloc(sizeReq);
  if (pv)
  {
    pTM = (TASK_MGMT *)pv;

    pTM->pstack = NULL;

    pTM->pData = (BYTE*)pv + blocksizeof(TASK_MGMT);

    pTM->pfnTask = pfnTask;
    pTM->pfnArgs = pfnArgs;

    pTM->name[NAMELEN] = 0;
    strncpy(pTM->name, name, NAMELEN);

    pthread_attr_init(&pTM->ThAttr);
    minPrio = sched_get_priority_min(SCHED_FIFO);
    if (minPrio == -1) {
       printf("[Error] sched_get_priority_min %s\n", strerror(errno));
       return -1;
    }
    maxPrio = sched_get_priority_max(SCHED_FIFO);
    if (maxPrio == -1) {
       printf("[Error] sched_get_priority_max %s\n", strerror(errno));
       return -1;
    }
 
    if (ulPrio < minPrio ||
          ulPrio > maxPrio) {
      printf("[Error] Invalid priority\n");
      return -1;
    }
 
    sp.sched_priority = ulPrio;
 
    rv = pthread_attr_setschedpolicy(&pTM->ThAttr, SCHED_FIFO);
    if (rv) {
       printf("[Error] pthread_attr_setinheritsched %s\n", strerror(errno));
       return rv;
    }
 
    rv = pthread_attr_setinheritsched(&pTM->ThAttr, PTHREAD_EXPLICIT_SCHED);
    if (rv) {
       printf("[Error] pthread_attr_setinheritsched %s\n", strerror(errno));
       return rv;
    }
 
    rv = pthread_attr_setschedparam(&pTM->ThAttr, &sp);
    if (rv) {
       printf("[Error] pthread_attr_setschedparam %s\n", strerror(errno));
       return rv;
    }
 
    rv = pthread_attr_setstacksize(&pTM->ThAttr, stackSize);
    if (rv) {
       printf("[Error] pthread_attr_setstacksize %s\n", strerror(errno));
       return rv;
    }

    rv = pthread_mutex_init(&pTM->taskMut, NULL);
    if (rv) {
       printf("[Error] pthread_mutex_init %s\n", strerror(errno));
       return rv;
    }

    rv = pthread_cond_init(&pTM->taskCond, NULL);
    if (rv) {
       printf("[Error] pthread_cond_init %s\n", strerror(errno));
       return rv;
    }
 
    rv = pthread_create(&pTM->taskTh, &pTM->ThAttr, pfnTask, pfnArgs);
    if (rv) {
       printf("[Error] pthread_create %s\n", strerror(errno));
       return rv;
    }
 
    rv = pthread_setschedprio(pTM->taskTh, ulPrio);
    if (rv) {
       printf("[Error] pthread_setschedprio %s\n", strerror(errno));
       return rv;
    }
 
    rv = pthread_setname_np(pTM->taskTh, name);
    if (rv) {
       printf("[Error] pthread_setname_np %s\n", strerror(errno));
       return rv;
    }

  }
  return 0;
}

int main(int argc, char *argv[])
{
  pthread_t tid;
  int tt = 0, ret;

  ret = getNewThread(&tid, INIT, initTask, SVR_PRIORITY, 2, &tt);
  if (ret != 0) {
    printf("[Error] failed to start thread\n");
    return ret;
  }

  pthread_join(tid, NULL);
  pthread_attr_destroy(&pgTM->ThAttr);
  MainCleanUp();
  return 0; //never come here
}
