/* Original version of this program by Frank Ruskey (1995) can be
   found at http://sue.uvic.ca/~cos/inf/perm/PermInfo.html -- a subpage of
   "The Combinatorial Object Server"
*/
#include <stdio.h>
#include <stdlib.h>

void Perm( int );

int  *p, *pi;       /* The permutation and its inverse  */
int  *dir;          /* The directions of each element   */
int N;              /* returns permutations from 1 to N */
int z, j, jd;       /* temporaries for swapping values  */


void main (void)
   {
   int i, n;

   printf ("Enter n: ");  scanf ("%d", &n);
   printf ("\n");
   N = n;

   p   = malloc ((n+1) * sizeof (int));
   pi  = malloc ((n+1) * sizeof (int));
   dir = malloc ((n+1) * sizeof (int));

   for (i=1; i <= n; ++i)
      {
      dir[i] = -1;
      p[i] = pi[i] = i;
      }

   Perm( 1 );
   }


void Perm( int n )
   { 
   int i;

   if (n > N)
      {
      for (i=1; i <= N; ++i)
         printf ("%d  ", p[i]);
      printf ("\n");
      }
   else
      {
      Perm( n+1 );
      for (i=1; i <= n-1; i++)
         {
         j = pi[n];
         jd = j + dir[n];
         z = p[jd];
         p[j] = z;
         p[jd] = n;
         pi[z] = j;
         pi[n] = jd;
         Perm( n+1 ); 
         }
      dir[n] = -dir[n];
      }
   }
