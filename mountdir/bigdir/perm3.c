
/*
   Dijkstra's Permutation Generating Algorithm:
     - Generates them in dictionary order
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int main(int argc, char *argv[])
{
  int i, j, r, s, temp, n;
  int *pi;

  n = atoi(argv[1]);

  pi = malloc((n+1) * sizeof( int ) );
  for(i = 0; i <= n; i++)
    pi[i] = i;

  i = 1;
  while (i) {
    for (i=1; i <= n; i++) printf( " %2d", pi[i] );
    printf( "\n" );

    i = n-1;
    while (pi[i] > pi[i+1]) i--;

    j = n;
    while (pi[i] > pi[j]) j--;

    temp = pi[i];
    pi[i] = pi[j];
    pi[j] = temp;

    r = n;
    s = i+1;
    while (r > s){
      temp = pi[r];
      pi[r] = pi[s];
      pi[s] = temp;
      r--; s++;
    }
  }
}
