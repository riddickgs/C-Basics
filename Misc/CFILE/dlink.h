#ifndef _DLINK_H_
#define _DLINK_H_


typedef struct _dl {
  struct _dl *prev;
  struct _dl *next;
  void       *pUser;
} DLINK;

extern void DLinkInsert(void **pStart, DLINK *pNew);
extern void DLinkRemove(void **pStart, DLINK *pCur);
extern void DLinkReset(DLINK *pNode);
#endif
