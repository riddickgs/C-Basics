/* Stack - LIFO 
 * \Author: 		Riddick
 * \Description:	This is sample stack code
 * \Date		Thu Mar 21 13:15:01 IST 2024
 * 
 * Extras:
 * Stack using Array
 * Stack using Linked list 
 * */

#include <stdio.h>
#include <stdlib.h>
#include <limits.h>

#define CAPACITY 100
typedef struct stack {
#ifdef STACK_USING_ARRAY
  int top;
  unsigned int capacity;
  int *array;
#else
  int data;
  struct stack *next;
#endif
  struct stack *head;
  void (* create)(struct stack *st, unsigned int capacity);
  void (* push)(struct stack **st, int item);
  int (* pop)(struct stack **st);
  int (* peek)(struct stack *st);
  void (* display)(struct stack *st);
  void (* destroy)(struct stack **st);
} Stack;

/*
typedef struct stackll {
  int data;
  struct stackll *next;
} Stack;
*/
#ifdef STACK_USING_ARRAY
//create memory of stack
void create_stack(Stack *st, unsigned int cap) {
  //Stack *st = (Stack *)malloc(sizeof(Stack));
  st->capacity = cap;
  st->top = -1;
  st->array = (int *)malloc(st->capacity * sizeof(int));
  //return st;
}

//isEmpty
int isEmpty(Stack *st) {
  return st->top == -1;
}

//isFull
int isFull(Stack *st) {
  return st->top == st->capacity-1;
}

//push
void pusharr(Stack **st, int item) {
  if (isFull(*st)) return;
  printf("Pushed to stack (%d)\n", item);
  (*st)->array[++(*st)->top] = item;
}


//pop
int poparr(Stack **st) {
  if (isEmpty(*st)) return INT_MIN;
  printf("Popped from stack (%d)\n", (*st)->array[(*st)->top]);
  return (*st)->array[(*st)->top--];
}

//peek
int peekarr(Stack *st) {
	printf("Top of the stack (%d)\n", st->array[st->top]);
  return st->array[st->top];
}

//display whole stack
void displayarr(Stack *st) {
  printf("Members in stack\t[ ");
  for (int i = 0; i <= st->top; i++) {
    printf("%d ", st->array[i]);
  }
  printf("]\n");
}

//release memory of stack
void destroyarr(Stack **st) {
  free((*st)->array);
  free(*st);
}

#else
Stack *getNode(int item) {
  Stack *st = (Stack *)malloc(sizeof(Stack));
  st->data = item;
  st->next = NULL;
  return st;
}

int isEmptyll(Stack *st) {
  return !st;
}

void pushll(Stack **st, int item) {
  //get node
  Stack *new = getNode(item);
  printf("[LL]Pushed to stack (%d)\n", item);
  new->next = *st; //add at head
  *st = new;
}

int popll(Stack **st) {
  if (isEmptyll(*st)) return INT_MIN;
  Stack *temp = *st;
  int popp = temp->data;
  printf("[LL]Popped from stack (%d)\n", popp);
  *st = (*st)->next;
  free(temp);
  return popp;
}

void displayll(Stack *st) {
  printf("[LL]Members in stack\t[ ");
  Stack *temp = st;
  while (temp) {
    printf("%d ", temp->data);
    temp = temp->next;
  }
  printf("]\n");
}

int peekll(Stack *st) {
  printf("[LL]Top of the stack (%d)\n", st->data);
  return st->data;
}

void destroyll(Stack **st) {
  Stack *temp = (*st)->next, *fr;
  while (temp->next) {
    fr = temp;
    temp = temp->next;
    //fr->next = NULL;
    free(fr);
  }
  free(*st);
}
#endif

int main() {
  Stack *st = (Stack *)malloc(sizeof(Stack));
  {
#ifdef STACK_USING_ARRAY
    st->create = create_stack;
    st->push = pusharr;
    st->pop = poparr;
    st->peek = peekarr;
    st->display = displayarr;
    st->destroy = destroyarr;
    st->head = (Stack *)malloc(sizeof(Stack));
    st->create(st->head, CAPACITY);
#else
    st->push = pushll;
    st->pop = popll;
    st->peek = peekll;
    st->display = displayll;
    st->destroy = destroyll;
    st->head = NULL;
#endif
  }
  //printf("Stack using Array\n");
  st->push(&st->head, 10);
  st->push(&st->head, 20);
  st->push(&st->head, 31);
  st->push(&st->head, 32);
  st->push(&st->head, 40);
  st->push(&st->head, 2);
  st->push(&st->head, 21);
  st->push(&st->head, 60);
  st->push(&st->head, 33);
  st->pop(&st->head);
  st->peek(st->head);
  st->display(st->head);
  st->destroy(&st->head);
  return 0;
}
