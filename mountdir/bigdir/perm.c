
/*
  Found this on the net.
  No credit was given.
  It's correct.
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void visit (int);

int level = -1;
int N;
int *sig;

int main(int argc, char *argv[])
{
   int i;

   N = atoi(argv[1]);
   sig = (int *) malloc (N * sizeof (int));
   for (i=0; i < N; i++)
       sig[i] = 0;
   visit(0);
}
void visit (int k)
{
   int i;

   sig[k] = ++level;
   if (level == N)
      {
      for (i=0; i < N; i++) printf ("%2d", sig[i] - 1);
      printf ("\n");
      }
   else
      for (i = 0; i < N; i++)
         if (sig[i] == 0) visit(i);

   level--;
   sig[k] = 0;
}
