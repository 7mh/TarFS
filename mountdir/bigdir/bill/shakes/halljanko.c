
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define N    315

typedef
  unsigned char
uchar;

int a[N][N];
int b[N][N];
int dist[N][N];
int pred[N];

int bitcount(unsigned int n);
void mkdlist(int dl[], int u);
void dfs(int dl[], int u, int dep);

int main(int argc, char *argv[])
{
   int i,j,k,l;

   for(i=0;i<N;++i)
      for(j=0;j<N;++j){
         scanf("%1d",&k);
         a[i][j] = k;
      }
   for(i=0;i<N;i++)
     mkdlist(dist[i], i);

   memset(b, 0, N*N*sizeof(int));
   for(i=0;i<N;++i)
      for(j=0;j<N;++j)
         if(dist[i][j] == 1 || dist[i][j] == 3 || dist[i][j] == 4)
            b[i][j] = 1;

   for(i=0;i<N;++i){
      for(j=0;j<N;++j)
         printf("%1d",b[i][j]);
      printf("\n");
   }
}
int bitcount(unsigned int n)
{
   int i;

   for(i=0;n;i++)
     n &= (n-1);
   return i;
}
void mkdlist(int dl[], int u)
{
  int i;

  for(i=0;i<N;i++)
    dl[i] = 0x7fffff;
  for(i=0;i<N;i++)
    pred[i] = -1;
  dl[u] = 0;
  dfs(dl,u,1);
}
void dfs(int dl[], int u, int dep)
{
  int i,v;

  for(v=0;v<N;v++){
    if(a[u][v] == 0) continue;
    if(dep < dl[v]){
      dl[v] = dep;
      pred[v] = u;
      dfs(dl,v,dep+1);
    }
  }
}

