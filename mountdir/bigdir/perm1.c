
/*
  This is pretty fast.  I created this one based on
  a lecture I heard while working at Schneider Logistics.
*/

#include <stdio.h>

#define N           10
#define NFACT  3628800

typedef unsigned char uchar;

uchar siglist[NFACT][N];
int nperms;

int main()
{
  makeperms();
  printperms();
  printf("%d permutations\n",nperms);
}
printperms()
{
  int i,j;

  for(i=0;i<NFACT;i++){
    for(j=0;j<N;j++)
      printf("%d",siglist[i][j]);
    printf("\n");
  }
}
makeperms()
{
/*
  p is the current permutation
  pinv is it's inverse
*/
  int p[N+2],pinv[N+2],d[N+2],sigma[N+2];
  int h,i,j,m;
  uchar done;

  for(i=1;i<=N+1;i++){
    p[i] = i;
    pinv[i] = i;
    d[i] = -1;
  }
  p[0] = N+1;
  sigma[1] = sigma[N+1] = 0;
  for(i=2;i<=N;i++)
    sigma[i] = 1;
  done = 0;
  nperms = 0;
  while(!done){
    for(i=0;i<N;i++)
      siglist[nperms][i] = p[i+1];
    nperms++;
    for(i=1;i<=N;i++)
      if(sigma[i])
        break;
    if(sigma[i]){
      m = N;
      while(!sigma[m])
        --m;
      j = pinv[m];
      p[j] = p[j+d[m]];
      p[j+d[m]] = m;
      pinv[m] = pinv[m] + d[m];
      pinv[p[j]] = j;
      if(m < p[j+2*d[m]]){
        d[m] = -d[m];
        sigma[m] = 0;
      }
      for(h=m+1;h<=N;h++)
        sigma[h] = 1;
    } else {
      done = 1;
    }
  }
  return nperms;
}
