/* Create a function that segregates even and odd values of an integer array using pointers.

WTD: Use two pointers, one starting from the beginning and the other from the end. Traverse and swap even and odd numbers until the two pointers meet.

(e.g.: I/P: int arr[] = {12,34,9,8,45,90} ,O/P: {12,34,8,90,9,45} )
 */
#include <stdio.h>
#include <stdlib.h>

#define IS_ODD(x) (x%2)

static inline void swap(int *s, int *e) {
  *s ^= *e;
  *e = *s ^ *e;
  *s = *s ^ *e;
}

int main() {
  int arr[] = {12,34,9,8,45,90}, *s, *e;
  s = &arr[0];
  e = s + sizeof arr / sizeof arr[0] - 1;

  while (s < e) {
    if (IS_ODD(*s)) {
      swap(s, e);
    } else {
      s++;
    }

    if (IS_EVEN(*e)) {
    
    }
  }
  return 0;
}
