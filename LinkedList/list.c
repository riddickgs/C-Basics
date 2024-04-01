#include <stdio.h>
#include <math.h>
#include "list.h"

#define LISTSIZE (sizeof(List))
static inline List *getNode(size_t size) {
  return (List *)malloc(size);
}

static inline int getItem() {
  int item;
  printf("Enter the data [0-1000]\n");
  scanf("%d", &item);
  return item;
}

static inline int getPos() {
  int pos;
  printf("Enter a position [1 - length of list]\n");
  scanf("%d", &pos);
  return pos;
}

extern void display(List *head) {
  List *t = head;

  printf("[ ");
  while (t->next) {
    printf("%d->", t->data);
    t = t->next;
  }
  printf("%d ]\n", t->data);
}

extern void destroy(List **head){
  List *t = *head, *f;

  while (t) {
    f = t;
    t = t->next;
    free(f); //free memory
  }
  *head = NULL;
}

extern void addToListAtBegin(List **head, int item) {
  List *n = getNode(LISTSIZE);
  n->data = item;
  if (*head)
    n->next = *head;
  *head = n;
}

extern void addToListAtEnd(List **head, int item) {
  List *n = getNode(LISTSIZE), *t = *head;
  n->data = item;
  n->next = NULL;

  if(*head) {
    while (t->next) {
      t = t->next;
    }
    t->next = n;
  } else {
    *head = n; //head null
  }
}

extern void addToListAtPos(List **head, int item, int pos) {
  int c = 1;
  List *p, *t = *head, *n = getNode(LISTSIZE);
  n->data = item;
  n->next = NULL;

  while (t) {
    if (c == pos) break;
    p = t;
    t = t->next;
    c++;
  }

  if (c != pos) {
    printf("Invalid position (%d), Please try again!!\n", pos);
    free(n);
    return;
  }
  //head
  if (t == *head) {
    n->next = *head;
    *head = n;
  } else if (!t) {
  //tail
    p->next = n;
    n->next = NULL;
  } else {
  //intermediate
    p->next = n;
    n->next = t;
  }

}

extern void removeFromListAtBegin(List **head) {
  List *t = *head;
  if (!t) return;
  *head = t->next;
  printf("Removing data (%d) from list\n", t->data);
  free(t);
}

extern void removeFromListAtEnd(List **head) {
  List *t = *head, *p;

  while (t->next) {
     p = t;
     t = t->next;
  }

  p->next = NULL;
  printf("Removing data (%d) from list\n", t->data);
  free(t);
}

extern void removeFromListAtPos(List **head, int pos) {
  List *t = *head, *p;
  int c = 1;

  while (t) {
    if (c == pos) break;
    p = t;
    t = t->next;
    c++;
  }
  
  if (!t || c != pos) {
    printf("Invalid position (%d), Please try again!!\n", pos);
    return;
  }

  if (t == *head) {
    *head = t->next;
  } else if (!t) {
    p->next = NULL; 
  } else {
    p->next = t->next;
  }
  printf("Removing data (%d) from list\n", t->data);
  free(t);
}

extern void reverseList(List **head) {
  List *t = *head, *p, *q = NULL;
  while (t) {
    p = t->next;  // save the next
    t->next = q;  // set the next direction
    q = t;        // point to previous node
    t = p;        // get the next node
  }
  *head = q;      // update the head of the list 
}

/* reverse using recursion*/
#if 0
extern List *reverse(List *node) {
  if (node == NULL) return NULL;
  if (node->next == NULL) {
    *head = node;
    return node;
  }

  List *node1 = reverse(node->next);
  node1->next = node;
  node->next = NULL;
  return node;
}
#endif

extern void addToList(List **head, int wh) {
  if (wh == BEGIN) addToListAtBegin(head, getItem());
  else if (wh == END) addToListAtEnd(head, getItem());
  else addToListAtPos(head, getItem(), getPos());
}

extern void removeFromList(List **head, int wh) {
  if (wh == BEGIN) removeFromListAtBegin(head);
  else if (wh == END) removeFromListAtEnd(head);
  else removeFromListAtPos(head, getPos());
}

static List *getMiddleList(List *head) {
  List *s = head, *f = head->next;

  while (f) {
    f = f->next;
    if (f) {
      s = s->next;
      f = f->next;
    }
  }
  return s;
}

extern void middleList(List *head) {
  List *t = getMiddleList(head);
  printf("Middle of the List (%d)\n", t->data);
}

/* Lenght of the list */
extern int lengthList(List *head) {
  List *t = head;
  int c = 0;
  while (t) {
    t = t->next;
    c++;
  }
  return c;
}

extern void getSampleList(List **head, int size) {
  int i;
  for (i = 0; i < size; i++) {
    addToListAtBegin(head, rand()%100+1);
  }
}

extern void getSortedSampleList(List **head, int size) {
  getSampleList(head, size);
  MergeSort(head);
}
static inline char *getsuf(const int pos) {
  int rem = pos % 10;
  switch (rem) {
	  case 1:
		  return "st";
		  break;
	  case 2:
		  return "nd";
		  break;
	  case 3:
		  return "rd";
		  break;
	  case 4:
	  case 5:
	  case 6:
	  case 7:
	  case 8:
	  case 9:
	  case 0:
		  return "th";
		  break;
  }
  return NULL;
}
/* Remove duplicate from an unsorted linked list */
//TODO

/* find nth node from the end of the linked list */
extern void findNthFromEnd(List *head, int pos) {
  List *t = head;
  int len = lengthList(head), c = 0;
  if (!pos) {
    printf("Invalid position (%d), Please try again!!\n", pos);
    return;
  }
  while (t) {
    if ((len - c) == pos) break;
    t = t->next;
    c++;
  }
  printf("%d%s node from end is (%d)\n", pos, getsuf(pos), t->data);
}

/* Move last element to the first of the linked list */
extern void movePosToFirst(List **head, int pos) {
  List *t = *head, *p;
  int c = 1;
  if (!pos) {
    printf("Invalid position (%d), Please try again!!\n", pos);
    return;
  }

  while (t) {
    if (c == pos) break;
    p = t;
    t = t->next;
    c++;
  }

  if (c != pos) {
    printf("Invalid position (%d), Please try again!!\n", pos);
    return;
  }
  //got the pos node
  p->next = t->next;
  t->next = *head;
  *head = t;
}

extern void moveLastToFirst(List **head) {
  int len = lengthList(*head);
  movePosToFirst(head, len);
}

/* Delete alternate nodes of the linked list */
extern void deleteAltNodes(List *head) {
  List *t = head, *f;

  while (t->next && t->next->next) {
    f = t->next;
    t->next = t->next->next;
    free(f);
    t = t->next;
  }
}

/* pairwise swap elements of the linked list */
extern void swapPairWise(List **head) {
  List *t = *head, *f = NULL, *ff;
  *head = t->next;
  while (t) {
    ff = t->next; //saving next->next node
    t->next = ff->next; //update next to next->next
    ff->next = t;
    if (f) {
      f->next = ff;
    }
    f = t;        //retain the prev to update on next iteration
    t = t->next;
  }
}

/* find if cycle is present in linked list. get the started node */
extern int isCyclePresent(List *head) {
  List *s = head->next, *f=head->next->next;
  while (s && f && f->next) {
    if (s == f) break;
    s = s->next;
    f = f->next->next;
  }
  /* find the starting node if cycle present */
  if (s != f)
    return 0;
  s = head;
  while (s != f) {
    s = s->next;
    f = f->next;
  }
  printf("Membes of cycle in linked list %d->", s->data);
  //members of cycle
  s = s->next;
  while (s!=f) {
    printf("%d->", s->data);
    s = s->next;
  }
  printf("\n");
  return 1;
}

/* Intersection of two linked list */
extern List *findIntersectionList(List *h1, List *h2) {

  while (h2) {
    List *t = h1;
    while (t) {
      if (t == h2) return h2;
      t = t->next;
    }
    h2 = h2->next;
  }
  return NULL;
}

/* Saggregate even and odd nodes |make even and odd elements together|*/
extern void separateEvenOddElements(List **head) {
  List *t = *head, *oh = NULL, *on, *eh = NULL, *en;

  //1->2->2->1
  while (t) {
    if (t->data % 2) { // number is odd
      if (oh) {
        on->next = t;
	on = on->next;
      }
      else
        oh = on = t;
    } else {
      if (eh) {
        en->next = t;
	en = en->next;
      }
      else
        eh = en = t;
    }
    t = t->next;
  }
  *head = eh;
  en->next = oh;
  on->next = NULL;
}

static List *sortedMerge(List *a, List *b) {
  List *result;
  //base case
  if (!a) return b;
  else if (!b) return a;

  if (a->data <= b->data) {
    result = a;
    result->next = sortedMerge(a->next, b);
  } else {
    result = b;
    result->next = sortedMerge(a, b->next);
  }
  return result;
}
/* Merge sort linked list */
extern void MergeSort(List **root) {
  //Steps for Merge sort
  //1. split into 2 list
  //2. recursively call both the list to sort
  //3. merge the list
  List *head = *root, *a, *b, *m;
  //base case
  if (!head || !head->next)
    return;
  m = getMiddleList(head);
  a = head;
  b = m->next;
  m->next = NULL;
  MergeSort(&a);
  MergeSort(&b);
  *root = sortedMerge(a, b);
}

/* merge two sorted linked list, final list should be sorted */
extern void mergeSortedList(List **l1, List *l2) {
  List *t = *l1, *p = NULL, *n;

  // loop through both the list
  while (l2 && t) {
    if (l2->data <= t->data) {
      n = l2->next;
      if (p) p->next = l2;
      else *l1 = l2;
      l2->next = t;
      l2 = n;
    }
    p = t;
    t = t->next;
  }
}

static void getInput(List **l) {
  int num, rem;
  scanf("%d", &num);
  while (num) {
    rem = num % 10;
    addToListAtBegin(l, rem);
    num /= 10;
  }
}
/* Add two number represented by the linked list */
extern void getSum() {
  // l1 and l2 represent the nnumber and result will be returned
  List *t1, *t2, *l1 = NULL, *l2 = NULL, *l3 = NULL;
  int num = 0;
  getInput(&l1);
  printf("First number is:\t");
  display(l1);
  getInput(&l2);
  printf("Second number is:\t");
  display(l2);
  reverseList(&l1); //4->2
  reverseList(&l2); //8->6
  t1 = l1;
  t2 = l2;
  while (t1 && t2) {
    num  = t1->data + t2->data + num/10;
    addToListAtBegin(&l3, num%10);
    t1 = t1->next;
    t2 = t2->next;
  }

  if (t1)
    addToListAtBegin(&l3, t1->data);
  else if(t2)
    addToListAtBegin(&l3, t2->data);
  else if (num/10)
    addToListAtBegin(&l3, num/10);
  printf("Sum of two numbers are:\t");
  display(l3);

  destroy(&l1);
  destroy(&l2);
  destroy(&l3);
}

/* find sum of two linked list using stack */

/* compare two string represented by linked list */

/* clone a linked list using next and random pointer */

/* Flatten multi-level linked list */

/* Partition a linked list around a given value */

/* Remove all nodes from a linked list having specific value */

/* convert a binary number represented by a linked list into a integer */
extern List *getDecimalToBinary(int n) {
  List *t = NULL;
  while(n) {
    addToListAtBegin(&t, n%2);
    n /= 2;
  }
  return t; //free t from main
}

extern int getBinaryToDecimal(List *l) {
  reverseList(&l);
  List *t = l;
  int e = 0;
  int val;
  while (t) {
    val += t->data * (int)pow(2, e++);
    t = t->next;
  }
  return val;
}

/* Find the common ancestor of two nodes in  a binary tree represented as a doubly linked list */

/* determine a linked list is a palindrome */
extern int isListPalindrome(List *head) {
  List *t1 = head, *t2, *m = getMiddleList(head);
  int len = lengthList(head);
  if (len%2) m = m->next; //if odd then go with mid + 1
  t2 = m;
  reverseList(&t2);  // 2->1
  m = t2; // saving the middle pointer to reverse again

  while (t1 && t2) {
    if (t1->data != t2->data) return 0;
    t1 = t1->next;
    t2 = t2->next;
  }

  reverseList(&m);  // back to original list
  return 1;
}
