/* { dg-do run { target powerpc*-*-* } } */
/* { dg-do run { target i?86-*-* x86_64-*-* } } */
/* { dg-options "-funroll-loops -O2 -ftree-vectorize -fdump-tree-vect-stats -maltivec" { target powerpc*-*-* } } */
/* { dg-options "-funroll-loops -O2 -ftree-vectorize -fdump-tree-vect-stats -msse2" { target i?86-*-* x86_64-*-* } } */

#include <stdarg.h>
#include "tree-vect.h"

#define N 8
 
int main1 ()
{  
  int b[N] = {0,3,6,9,12,15,18,21};
  int a[N];
  int i;
  
  for (i = 0; i < N; i++)
    {
      a[i] = b[i];
    }
  
  /* check results:  */
  for (i = 0; i < N; i++)
    {
      if (a[i] != b[i])
	abort ();
    }
  
  return 0;
}

int main (void)
{ 
  check_vect ();

  return main1 ();
} 

/* { dg-final { scan-tree-dump-times "vectorized 1 loops" 1 "vect" } } */
