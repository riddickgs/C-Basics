#include "dlink.h"


extern void DLinkInsert(void **pStart, DLINK *pNew)
{
   //Disable preemption
   if (!(pNew->prev || pNew->next || (*pstart == pNew))) //Check that node is not in list
   {
     pNew->prev = NULL;

     pNew->next = *pStart;
     if (pNew->next)
       pNew->next->prev = *pNew;

     *pStart = pNew;
   }
   //Enable preemption
}

extern void DLinkRemove(void **pStart, DLINK *pCur)
{
   //Disable preemption
   if (pCur->prev || pCur->next || (*pStart == pCur))  //Check that cur node is in list
   {
      if (pCur->prev)
      {
        pCur->prev->next = pCur->next;
      }
      else
      {
        *pstart = pCur->next;
      }

      if (pCur->next)
      {
        pCur->next->prev = pCur->prev;
      }

      pCur->prev = pCur->next = NULL;
   }
   //Enable preemption

}
extern void DLinkReset(DLINK *pNode)
{
  pNode->prev = pNode->next = NULL;
}

