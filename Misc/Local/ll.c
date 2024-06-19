#include <stdio.h>
#include <stdlib.h>
#define true 1
#define false 0
typedef struct ll {
  int data;
  struct ll *next;
} List;

int getItem() {
  int item;
  printf("Enter an item [0-1000]\n");
  scanf("%d", &item);
  return item;
}

int getPos() {
  int pos;
  printf("Enter the position to add\n");
  scanf("%d", &pos);
  return pos;
}

void createlist(List **l) {
  *l = NULL;//(List *)malloc(sizeof(List *));
}

List *getNode() {
  List *l = (List *)malloc(sizeof(List));
  l->next= NULL;
  return l;
}

void insertAtBegin(List **l, int item) {
  //get a new node
  List *n = getNode();
  //save the data
  n->data = item;
  //head is present
  if (*l) {
    n->next = *l;
    *l = n;
  } else {
    *l=n;
  }
}

void insertAtEnd(List **l, int item) {
  List *n = getNode(), *t = *l;
  n->data = item;
  while (t->next) {
    t = t->next;
  }
  
  t->next = n;
}

void insertAtPos(List **l, int item, int pos) {
  List *n = getNode(), *t = *l, *p;
  int c = 1;
  n->data = item;

  if (!pos) {
    printf("Invalid position!! Please try again!!\n");
    return;
  }
  
  while (t) {
    if (c == pos) break;
    p = t;
    t = t->next;
    c++;
  }
  if (c > pos) {
    printf("Invalid position!! Reached end of list!!\n");
  } 

  if (t == *l) { // head
    n->next = *l;
    *l = n;
  } else if (!t->next) { //end
    t->next = n;
  } else { //intermediate
    n->next = t;
    p->next = n;
  }
}

void removeAtBegin(List **l) {
  if (!*l) {
    printf("List is empty\n");
    return;
  }
  List *t = *l;
  *l = t->next;
  printf("Removing item (%d)\n", t->data);
  free(t);
}

void removeAtEnd(List **l) {
  if (!*l) {
    printf("List is empty\n");
    return;
  }

  List *t = *l, *p;

  while (t->next) {
    p = t;
    t = t->next;
  }

  p->next = NULL;
  
  printf("Removing item (%d)\n", t->data);
  free(t);
}

void removeAtPos(List **l, int pos) {
  if (!*l) {
    printf("List is empty\n");
    return;
  }

  if (!pos) {
    printf("Invalid position!!\n");
    return;
  }
  
  List *t = *l, *p;
  int c = 1;
  while (t->next) {
    if (c == pos) break;
    p = t; 
    t = t->next;
    c++;
  }

  if (*l == t) {
    //remove the head
    *l = t->next;
  }else if (!t->next) {
    //last node
    p->next = NULL;
  } else {
    //intermediate node
    p->next = t->next;
  }
  printf("Removing item %d from position %d\n", t->data, c);
  free(t);
}

void display(List *l) {
  List *t = l;

  while (t) {
    printf("%d->", t->data);
    t = t->next;
  }
  printf("null\n");
}

void destroy(List **l) {
  List *t = *l, *f;

  if (!t) {
    printf("List is already destroyed!!\n");
    return;
  }

  while (t) {
    f = t;
    t = t->next;
    free(f);
  }
  *l = NULL;
}

int main() {
   List *l = NULL;
   int ch;
   int done = true;

  //create a linked list
  //operation on linked list
   printf("This is linked list utility:");
   while (done) {
     printf("Please enter the following choice:\n"
		   "\t0. Exit program\n"
		   "\t1. Create list\n"
		   "\t2. Add item\n"
		   "\t3. Remove item\n"
		   "\t4. Display list\n"
		   "\t5. Delete list\n");
     scanf("%d", &ch);
     switch (ch) {
        case 0:
		done = false;
		break;
   	case 1:
		createlist(&l);
		break;
	case 2:
		{
		int loop=true;
		int addch;
		while (loop) {
		  printf("Please enter the following choice:\n"
				"\t0. Back to main program\n"
				"\t1. Add at head\n"
				"\t2. Add at end\n"
				"\t3. Add at custom position\n");
	 	  scanf("%d", &addch);
	          switch(addch) {
			case 0:
				loop=false;
				break;
			case 1:
				insertAtBegin(&l, getItem());
				break;
			case 2:
				insertAtEnd(&l, getItem());
				break;
			case 3:
				insertAtPos(&l, getItem(), getPos());
				break;
			default:
				printf("Invalid Choice. Please try again!!\n");
				break;
		
		  }
		}
		break;
     		}
	case 3:
		{
		int loop = true;
		int choice;
		while (loop) {
		  printf("Please enter the following choice:\n"
				"\t0. Back to main program\n"
				"\t1. Remove at head\n"
				"\t2. Remove at end\n"
				"\t3. Remove at custom position\n");
		  scanf("%d", &choice);
		  switch (choice) {
		  	case 0:
				loop = false;
				break;
			case 1:
				removeAtBegin(&l);
				break;
			case 2:
				removeAtEnd(&l);
				break;
			case 3:
				removeAtPos(&l, getPos());
				break;
			default:
				printf("Invalid Choice. Please try again!!\n");
				break;
		  }
		}
		break;
	        }
        case 4:
		display(l);
		break;
	case 5:
		destroy(&l);
		break;
	default:
		printf("Invalid input.Please try again!!\n");
		break;
    }
  }
  destroy(&l);
  return 0;
}
