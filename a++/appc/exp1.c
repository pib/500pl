#include <stdio.h>

main()
{
   int ivar;         // integer variable
   int jvar;         // integer variable
   int *pvar;        // pointer to an integer variable

   ivar = 5;         // initializing ivar
   jvar = 7;         // initializing jvar
   pvar = &ivar;     // making pvar point to ivar

   printf("ivar: %d, pvar->%d\n", ivar, *pvar);

   pvar = &jvar;     // making pvar point to jvar

   printf("ivar: %d, pvar->%d\n", ivar, *pvar);
}

