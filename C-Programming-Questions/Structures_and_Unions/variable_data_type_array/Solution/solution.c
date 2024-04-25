#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef enum {
  CHAR,
  INTEGER,
  FLOAT,
  INVALID
} DTYPE;

typedef struct {
  int size;
  union {
    char *c;
    int *i;
    float *f;
  };
} Array;

//returns number of characters
static inline int getInt(int **ptr) {
  char r;
  int c = 1, d, *p = NULL;

  while (scanf("%d%c", &d, &r) && r != '\n') {
    p = realloc(p, sizeof(int)*c);
    p[c-1] = d;
    c++;
  }
  *ptr = realloc(p, sizeof(int)*c);
  p[c-1] = d;
  return c;
}

static inline int getFloat(float **ptr) {
  char r;
  int c = 1;
  float d, *p = NULL;
  while (scanf("%f%c", &d, &r) && r != '\n') {
    p = realloc(p, sizeof(float)*c);
    p[c-1] = d;
    c++;
  }
  *ptr = realloc(p, sizeof(float)*c);
  p[c-1] = d;
  return c;
}
#define STR_SZ 256
static inline int getChar(char **ptr) {
  char arr[STR_SZ];
  int l;
  scanf("%s", arr);
  fgets(arr, STR_SZ, stdin);
  printf("[DEBUG] input = %s\n", arr);
  l = strlen(arr);
  l++;
  *ptr =(char *) malloc(l*sizeof(char));
  strncpy(*ptr, arr, l);
  printf("[DEBUG] length = %d\n", l);
  return l;
}

static int getData(int type, Array *a) {
  printf("[DEBUG] type = %d\n", type);
  if (type == INTEGER) 
    return getInt(&a->i);
  else if (type == FLOAT)
    return getFloat(&a->f);
  else
    return getChar(&a->c);
  return -1;
}

static void printData(int type, Array a) {
  switch (type) {
	case INTEGER:
	for(int i = 0; i < a.size; i++) {
    	  printf("%d, ", a.i[i]);
	}
	printf("\b\b]\n");
	break;
	case FLOAT:
	for(int i = 0; i < a.size; i++) {
    	  printf("%.2f, ", a.f[i]);
	}
	printf("\b\b]\n");
	break;
	break;
        default:
    	printf("%s", a.c);
	break;
  }
}

static void free_array(int type, Array a) {
  void *p;
  if (type == INTEGER)
    p = a.i;
  else if (type == FLOAT)
    p = a.f;
  else
    p = a.c;
  free(p);
}

int main() {
  static char *dtype[4] = {"Char", "Integer", "Float", "Invalid"};
  Array a;
  int t;
  printf("Enter datatype\n\t[1] char\n\t[2] Interger\n\t[3] float\n");
  scanf("%d", &t);
  t--; //reduce as index will be 0 1 2
  
  if (t < CHAR || t >= INVALID) {
     printf("Invalid data type\n");
     return -1;
  }

  printf("Enter data\n");
  a.size = getData(t, &a);

  printf("Array of %s: [", dtype[t]);
  printData(t, a);
  free_array(t, a);
  return 0;
}
