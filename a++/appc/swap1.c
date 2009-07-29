
#include <stdio.h>
 
void swapInt(int *a, int *b)
{
   int temp;

   temp = *a;
   *a = *b;
   *b = temp;
}            

int main()
{
   int i1   = 1;
   int i2   = 2;
		     
   printf("before: i1: %d, i2: %d\n", i1, i2);
   swapInt(&i1,&i2);
   printf("after:  i1: %d, i2: %d\n", i1, i2);
}                           
/* output of program:

   before: i1: 1, i2: 2
   after:  i1: 2, i2: 1*/
