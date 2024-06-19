#include <stdio.h>
#include <string.h>
#include <math.h>
#include <stdlib.h>

#define LEFT(a) (((a) == 'q') || ((a) == 'w') || ((a) == 'e') || ((a) == 'r') || ((a) == 't'))
#define RIGHT(a) (((a) == 'y') || ((a) == 'u') || ((a) == 'i') || ((a) == 'o') || ((a) == 'p'))

typedef struct typ {
    //int word;
    float gtime;
    float ctime;
} TYP;

TYP *sTime;
void calculateTime(int index, int w, float d, char *str) {
    //number of words d
    //time given d
    //input sentence str
    char *ptr = str;
    float sum = 0;
    int l = 0, r = 0;
    //sTime[index].word = w;
    sTime[index].gtime = d;
    //printf("%s|index %d gtime %.2f str %s words %d\n", __FUNCTION__, index, d, str, w);
    while (ptr && *ptr) {
      //printf("%c\n", *ptr);
      if (LEFT(*ptr)) {
        printf("left __%c__\n", *ptr);
       //left 0.2
       sum += 0.2 * pow(2, l);
       l++;
       r = 0;
       } else if (RIGHT(*ptr)) {
         printf("right __%c__\n", *ptr);
         //right 0.1
         sum += 0.1* pow(2, r);
         r++;
         l = 0;
       } else {
         //space 0.3
         printf("space __%c__\n", *ptr);
         sum += 0.3;
         l = 0;
         r = 0;
       }
       ptr++;
       printf("sum %.2f\n", sum);
    }
    //printf("sum %.2f\n", sum);
  sTime[index].ctime = sum;
}

int main() {

    /* Enter your code here. Read input from STDIN. Print output to STDOUT */    
    //get input nunber of sentences
    char inp[3000];
    int n, w, i;
    float d;
    scanf("%d", &n); //got n
    if (n > 10000 || n < 0) return -1;
    sTime = (TYP *)calloc(n, sizeof(TYP));
    for (i = 0; i < n; i++) {
        scanf("%d\n%[^\n]s", &w, inp);
	scanf("%f", &d);
        calculateTime(i, w, d, inp);
    }
    
    for (i = 0; i < n; i++) {
        if (sTime[i].gtime == sTime[i].ctime) {
            printf("GOOD, %0.2f\n", sTime[i].ctime);
        } else if (sTime[i].gtime < sTime[i].ctime) {
            printf("GREAT JOB, %0.2f\n", sTime[i].ctime);
        } else {
            printf("WORK HARDER, %0.2f\n", sTime[i].ctime);
        }
    }
    return 0;
}
