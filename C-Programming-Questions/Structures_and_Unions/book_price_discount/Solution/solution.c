/* Problem:
 * Create a structure for a book with title, author, and price. Implement a function to discount the book's price by a given percentage.

WTD: Design a function that applies a specified discount percentage to the book's price, updating its value accordingly.

(e.g: I/P: Title: "Pride and Prejudice", Author: "Austen", Price: $30, Discount: 15%; O/P: New Price: $25.5 )
*/


#include <stdio.h>

typedef struct {
  char title[32];
  char author[32];
  int price;       //in dollars
  int discount;    //in percentage
} book;

void getInput(book *b) {
  printf("Enter Title of the book\n");
  //scanf("%[^31]*s", b->title);
  fgets(b->title, 32, stdin);
  printf("Enter Author of the book\n");
  //scanf("%[^31]*s", b->author);
  fgets(b->author, 32, stdin);
  printf("Enter price of the book in dollars($)\n");
  scanf("%d", &b->price);
  printf("Enter discount on the book\n");
  scanf("%d", &b->discount);
  printf("[DEBUG] I/P: Title: %s, Author: %s, Price: $%d, Discount: %d%%\n",
		  b->title, b->author, b->price, b->discount);
}

float calDiscount(book b) {
  return b.price - (float)b.discount/100 *b.price;
}

int main() {
  book b;
  int d;
  getInput(&b);
  printf("New Price: $%.1f\n", calDiscount(b));
  return 0;
}
