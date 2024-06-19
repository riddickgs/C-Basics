/* Bucket Sort:
WTD: Sort floating-point numbers using Bucket Sort.

Bucket Sort: Distributes elements into buckets, sorts individual buckets, and then concatenates them.

(e.g.: I/P: [0.897, 0.565, 0.656, 0.1234, 0.665, 0.3434]; O/P: [0.1234, 0.3434, 0.565, 0.656, 0.665, 0.897])
 * */

#include <stdio.h>
#include <stdlib.h>
#define GETNODE(x) ((malloc(sizeof(List)))->n=x)
typedef list {
   float n;
   struct list *next;
} List;

void insert(List **head, float data) {
  if (*head == NULL) {
    *head = GETNODE(data);
  } else {
    List *temp = *head;
    while(temp) { //new data will be sorted in an ascending order
      if (data < temp->n) {
        prev->next = GETNODE(data);
	prev->next->next = temp;
      }
      prev = temp;
      temp = temp->next;
    }
  }
}

void bucket_sort(float *arr, int n) {
  int i = 0, bi;
  List *b[n] = {NULL};
  //1. multiply all elements with the sizeof the array
  //2. push it to the index of new array of linked lists
  //3. sort individual linked list
  for (i = 0; i < n; i++) {
    bi = arr[i] * n;
    insert(&b[bi], arr[i]);
  }
  //4. remerge them back to the array
  int idx = 0;

  for(i=0; i < n; i++) {
  
    for (j =0; j < )
  }
}

int main() {
  //gte the array to sort
  float arr[] = {0.897, 0.565, 0.1234, 0.665, 0.3434};

  bucket_sort(&arr, sizeof arr/sizeof arr[0]);
  return 0; 
}
