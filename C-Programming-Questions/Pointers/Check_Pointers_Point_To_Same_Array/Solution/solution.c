/*Design a function that determines if two pointers point to the same array.
                                                                                                                                 WTD: Traverse from both pointers in both forward and backward directions until null or boundary is hit. If both pointers hit the same boundaries, they belong to the same array.                                                                                                                                                                                                                   (e.g.: I/P: int arr[] = {1,2,3,4}, *ptr1 = &arr[1], *ptr2 = &arr[3]; O/P: True ) */

#include <stdio.h>

//Check whether both pointer lies in between array boundary
int checkSameBoundary(int *start, int *end, int *p1, int *p2) {
  if ((p1 >= start && p1 <= end) && (p2 >= start && p2 <=end))
    return 1;
  return 0;
}

int main() {
  int arr[] = {1, 2, 3, 4}, *ptr1 = &arr[1], *ptr2 = &arr[3];

  printf("%s\n", (checkSameBoundary(arr, arr+ sizeof arr / sizeof arr[0], ptr1, ptr2))?"True":"False");
  return 0;
}
