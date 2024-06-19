#ifndef _NEWTOOL_H_
#define _NEWTOOL_H_

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>
#include "dlink.h"

/*define section - extern, structure, enum, typedefs*/
#define NAMELEN 15
#define DEFBASENAME "NT"
#define INIT "init"
#define CLOCK_PER_SEC 1000

#define ROOT_PRIORITY 99 //max priority
#define SVR_PRIORITY  44

#define blocksizeof(a) ((sizeof(a)+1)&~1)
#define blocksize(a)   ((a+1)&~1)
/*\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/
typedef unsigned char BYTE;
typedef struct {
  DLINK           dLink;  //list of task
  unsigned        uSysTId; //system generated thread it using SYS_getid()
  void            *(*pfnTask)(void*);
  void            *pfnArgs;
  void            *pstack;
  void            *pData;

  pthread_t       taskTh;
  pthread_attr_t  ThAttr;
  pthread_mutex_t taskMut;
  pthread_cond_t  taskCond;

  BYTE            bPreemptible;  // task preemptible flag
  BYTE            bSuspend;      // suspend flag
  BYTE            bPreemption;   // preemption flag
  BYTE            bRunStatus;
  
  unsigned long   ulPriority;
  long            lNiceness;
  char            name[NAMELEN+1];
} TASK_MGMT;

#endif
