
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define K  1024

// #define COLORING_CONDITION

// #define DAN_FORMAT

typedef
  unsigned char
uchar;

typedef struct vertex {
  int deg;
  int *nbr;
} VERTEX;

uchar **a;
VERTEX *b;
int good,mono;
int *sig;
int *used;
int first;

char fname[K];
FILE *colorfile;

int N,NC;
int calls;

void dfs(int v, int color[]);
void findsig();

int main(int argc, char *argv[])
{
  int i,j,k,m,u,v;
  int *color;
  FILE *fd;

  if(argc != 2){
    fprintf(stderr,"usage: %s graphfile\n",argv[0]);
    exit(0);
  }
  fd = fopen(argv[1],"r");
  if(!fd){
    fprintf(stderr,"cannot open %s\n",argv[1]);
    exit(0);
  }
  fscanf(fd,"%d %d %d", &N, &NC, &first);
  fprintf(stderr,"reading %s for a graph of order %d, coloring with %d colors, vertices numbered from %d\n",
    argv[1],N,NC,first);
  sig   = malloc(N*sizeof(int));
  used  = malloc(N*sizeof(int));
  color = malloc(N*sizeof(int));
  a = malloc(N*sizeof(uchar *));
  b = malloc(N*sizeof(VERTEX));
  for(i=0;i<N;i++){
    a[i] = malloc(N*sizeof(uchar));
    b[i].deg = 0;
    b[i].nbr = malloc((N-1)*sizeof(int));
  }
  m = 0;
  while(fscanf(fd,"%d %d", &u, &v) == 2){
    m++;
    if(first){
      u -= first;
      v -= first;
    }
    a[u][v] = a[v][u] = 1;
    b[u].nbr[b[u].deg++] = v;
    b[v].nbr[b[v].deg++] = u;
  }
  fclose(fd);
  fprintf(stderr,"%d edges read\n", m);
  for(i=0;i<N;i++)
    sig[i] = i;
  good = calls = 0;
  for(i=0;i<N;i++)
    color[i] = -1;
  memset(used,0,NC*sizeof(int));
  sprintf(fname,"colorings%d.txt",N);
  colorfile = fopen(fname,"w");
  if(!colorfile) colorfile = stdout;
  dfs(0,color);
  fprintf(stderr,"%d colorings in %s\n", good, fname);
}
int findbest(uchar used[])
{
  int i,j,k,m,u,v;

  m = -1;
  for(i=0;i<N;i++){
    if(used[i]) continue;
    k = 0;
    for(j=0;j<b[i].deg;j++){
      u = b[i].nbr[j];
      if(used[u])
        k++;
    }
    if(k > m){
      m = k;
      v = i;
    }
  }
#ifdef DEBUG
  fprintf(stderr,"%2d%4d\n",v,k);
#endif
  return (m >= 0) ? v : -1;
}
void findsig()
{
  int i,u;
  uchar *us;

  us = malloc(N);
  memset(us,0,N);
  sig[0] = 0; us[0] = 1;
  sig[1] = 1; us[1] = 1;
  for(i=2;i<N;i++){
    u = findbest(us);
    sig[i] = u;
    us[u] = 1;
  }
  free(us);
}
void dfs(int k, int color[])
{
  int i,j,u,v,c,ok;

  if(k == N){
    ++good;
#ifdef COLORING_CONDITION
    if(color[12] != color[13])
      return;
#endif
#ifdef DAN_FORMAT
    for(i=0;i<NC;i++){
      for(j=0;j<N;j++)
        if(color[j] == i)
          fprintf(colorfile," %d",j);
      fprintf(colorfile,"\n");
    }
    for(i=0;i<N;i++)
      fprintf(colorfile,"%d",color[i]);
    fprintf(colorfile,"\n");
#else
#endif
    return;
  }
  v = sig[k];
  if(color[v] != -1){
    fprintf(stderr,"sig[%d] == %d already colored: %d\n",k,v,color[v]);
    exit(0);
  }
  for(c=0;c<NC;c++){
    ok = 1;
    for(i=0;i<b[v].deg;i++){
      u = b[v].nbr[i];
      if(color[u] == c){
        ok = 0;
        break;
      }
    }
    if(ok){
      color[v] = c;
      used[c]++;
      dfs(k+1,color);
      used[c]--;
    }
    if(!used[c])
      break;
  }
  color[v] = -1;
}
