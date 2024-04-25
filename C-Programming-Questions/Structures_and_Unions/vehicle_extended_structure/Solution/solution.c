#include <stdio.h>
#include <stdlib.h>

typedef struct {
  int weight;    //in kg
  int speed;     //in kmph
} attribute;     //base structure

typedef struct {
  attribute attr;
  int seat_cap;
} car;           //derived structure

typedef struct {
  attribute attr;
  int max_load;
} truck;         //derived structure

typedef enum {
  CAR,
  TRUCK,
  INVALID_TYPE
} v_type;        //constants

void printinfo(int type, void *vptr) {
  truck *v = (truck *)vptr;

  if (type == TRUCK) {
    printf("Can load up to %dkg\n", v->max_load);
    printf("[DEBUG] Truck, Weight: %dkg Speed: %dkm/h, Load: %dkg\n",
		    v->attr.weight, v->attr.speed, v->max_load);
   }else {
    printf("Can seat up to %d people\n", ((car *)v)->seat_cap);
    printf("[DEBUG] Car, Weight: %dkg Speed: %dkm/h, Seating capacity: %d people\n",
		    v->attr.weight, v->attr.speed, ((car *)v)->seat_cap);
   }
}

int main() {
  int type, l;
  attribute at;
  void *v;

  printf("Enter the type of v\n\t[0] Car\n\t[1] Truck\n");
  scanf("%d", &type);
  if (type < CAR || type >= INVALID_TYPE) {
    printf("Invalid type of v selected!\n");
    return -1;
  }

  if (type == TRUCK) {
    printf("Enter Weight(in kg), Speed(in kmph), Load(in kg)\n");
    v = malloc(sizeof(truck));
  } else {
    printf("Enter Weight(in kg), Speed(in kmph), Seating Capacity\n");
    v = malloc(sizeof(car));
  }
  scanf("%d %d %d", &at.weight, &at.speed, &l);
  if (type == TRUCK) {
    truck *tr = (truck *)v;
    tr->attr = at;
    tr->max_load = l;
  } else {
    car *c = (car *)v;
    c->attr = at;
    c->seat_cap = l;
  }
  printinfo(type, v);
  free(v);
  return 0;
}
