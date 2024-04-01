#ifndef __LIST_H__
#define __LIST_H__
#include <stdio.h>
#include <stdlib.h>
/* ------------------ Variables -------------------------- */
typedef struct ll {
  int data;
  struct ll *next;
} List;

enum {
  BEGIN,
  END,
  INTERIM
};

/* ------------------- Function --------------------------- */
extern void display(List *head);
extern void destroy(List **head);
extern void addToListAtBegin(List **head, int item);
extern void addToListAtEnd(List **head, int item);
extern void addToListAtPos(List **head, int item, int pos);
extern void removeFromListAtBegin(List **head);
extern void removeFromListAtEnd(List **head);
extern void removeFromListAtPos(List **head, int pos);
extern void reverseList(List **head);
extern void addToList(List **head, int pos);
extern void removeFromList(List **head, int pos);
extern void middleList(List *head);
extern int lengthList(List *head);
extern void getSampleList(List **head, int size);
extern void getSortedSampleList(List **head, int size);
extern int isCyclePresent(List *head);
extern void deleteAltNodes(List *head);
extern void swapPairWise(List **head);
extern void findNthFromEnd(List *head, int pos);
extern void moveLastToFirst(List **head);
extern void movePosToFirst(List **head, int pos);
extern int isListPalindrome(List *head);
extern void separateEvenOddElements(List **head);
extern void MergeSort(List **head);
extern void mergeSortedList(List **l1, List *l2); 
extern void getSum();
extern List *getDecimalToBinary(int n);
extern int getBinaryToDecimal(List *l);
#endif
