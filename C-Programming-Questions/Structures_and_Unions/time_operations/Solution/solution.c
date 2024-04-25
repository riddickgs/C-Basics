/* Problem:
 * Define a structure for representing time (hours, minutes, seconds). Implement functions to add time durations and convert this duration to seconds.

WTD: Define a structure that models time, detailing hours, minutes, and seconds. Introduce functions that can sum two time durations and also convert a given duration into its equivalent representation in seconds.

(e.g: I/P: Time1: 1h 20m, Time2: 0h 50m; O/P: Sum: 2h 10m, Seconds: 7800s )
*/

#include <stdio.h>
#include <stdlib.h>
#define SEC_PER_MIN 60U
#define MIN_PER_HOUR (SEC_PER_MIN*SEC_PER_MIN)

typedef struct {
  int hours;
  int minutes;
  int seconds;
} time;

void getTime(time *t) {
  char arr[16];
  memset(t, 0, sizeof(time));
  printf("Enter time in HHh MMs\n");
  fgets(arr, 16, stdin);
  if (!sscanf(arr, "%dh %dm", &t->hours, &t->minutes)) {
    perror("sscanf");
    exit(0);
  }
  printf("[DEBUG] I/P: %dh %dm\n", t->hours, t->minutes);
}

void sumTime(time a, time b, time *c) {
  c->minutes = (a.minutes+b.minutes)%SEC_PER_MIN;
  c->hours = a.hours+b.hours + (a.minutes+b.minutes)/SEC_PER_MIN;
  printf("O/P: Sum: %dh %dm, ", c->hours, c->minutes);
}

void printTime(time t) {
  printf("Seconds: %lds\n",(long)(t.hours * MIN_PER_HOUR)+(t.minutes*SEC_PER_MIN)+t.seconds);
}

int main() {
  time a, b, c;
  getTime(&a);
  getTime(&b);
  sumTime(a, b, &c);
  printTime(c);
  return 0;
}
