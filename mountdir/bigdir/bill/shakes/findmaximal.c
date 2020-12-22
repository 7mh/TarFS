
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <time.h>
#include <sys/resource.h>

#define DEBUG

#define N                8
#define S                8

#define X              115

typedef
  unsigned char
uchar;

typedef struct vertex {
  int maximal;
  int nedges;
  uchar a[S][S];
} GRAPH;

GRAPH g[X];

int edgecount(uchar a[S][S]);
int is_subgraph(int u, int v);
int dfs_subgraph(int dep, uchar used[], int li[], uchar a[S][S], uchar b[S][S]);

int main(int argc, char *argv[])
{
  int d,i,j,k,x;
  FILE *fd;

  fd = fopen(argv[1],"r");
  for(k=0;k<X;k++){
    for(i=0;i<S;i++){
      for(j=0;j<S;j++){
        if(fscanf(fd,"%1d",&x) != 1){
          fprintf(stderr,"bad data at graph %d, location %d,%d\n",k,i,j);
          exit(0);
        }
        g[k].a[i][j] = x;
      }
    }
    g[k].nedges = edgecount(g[k].a);
    g[k].maximal = 1;
#ifdef DEBUG
    fprintf(stderr,"%4d(%2d)\n", k, g[k].nedges);
#endif
  }
  fprintf(stderr,"read all graphs\n");
  for(i=0;i<X;i++){
    for(j=0;j<X;j++){
      if(i == j)
        continue;
      if(!g[j].maximal)
        continue;
      if(is_subgraph(i,j)){
        g[j].maximal = 0;
      }
    }
  }
  for(k=0;k<X;k++){
    if(!g[k].maximal)
      continue;
    for(i=0;i<S;i++){
      for(j=0;j<S;j++)
        printf("%1d",g[k].a[i][j]);
      printf("\n");
    }
    fprintf(stderr,"%4d%4d\n",k,g[k].nedges);
  }
}
void readbiggraph(uchar b[N][N], char *fname)
{
  FILE *fd;
  int i,j,k;

  fd = fopen(fname,"r");
  if(!fd){
    fprintf(stderr,"unable to open %s\n", fname);
    exit(0);
  }
  memset(b,0,N*N);
  for(i=0;i<N;i++)
    for(j=0;j<N;j++){
      fscanf(fd,"%1d",&k);
      b[i][j] = k;
    }
  fclose(fd);
}
void readsmallgraph(uchar b[S][S], char *fname)
{
  FILE *fd;
  int i,j,k;

  fd = fopen(fname,"r");
  if(!fd){
    fprintf(stderr,"unable to open %s\n", fname);
    exit(0);
  }
  memset(b,0,S*S);
  for(i=0;i<S;i++)
    for(j=0;j<S;j++){
      fscanf(fd,"%1d",&k);
      b[i][j] = k;
    }
  fclose(fd);
}
int dfs_subgraph(int dep, uchar used[], int li[], uchar a[S][S], uchar b[S][S])
{
  int i,j;
  uchar bad;

  if(dep == S)
    return 1;
  for(i=0;i<S;i++){
    if(used[i]) continue;
    bad = 0;
    for(j=0;j<dep;j++){
      if(b[j][dep] == 0) continue;
      if(a[li[j]][i] == 0){
        bad = 1;
        break;
      }
    }
    if(!bad){
      li[dep] = i;
      used[i] = 1;
      if(dfs_subgraph(dep+1,used,li,a,b))
        return 1;
      used[i] = 0;
    }
  }
  return 0;
}
int edgecount(uchar a[S][S])
{
  int i,j,k;

  k = 0;
  for(i=0;i<S-1;i++)
    for(j=i+1;j<S;j++)
      if(a[i][j])
        k++;
  return k;
}
int is_subgraph(int u, int v)
{
  int li[S];
  uchar used[S];

  if(g[u].nedges < g[v].nedges)
    return 0;
  memset(used,0,S);
  return dfs_subgraph(0,used,li,g[u].a,g[v].a);
}
