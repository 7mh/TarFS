
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#define RND(X) (random()%(X))
#define DEG(X) b[X].deg

#define MP 32768
#define MD  1024

typedef
  unsigned char
uchar;

struct vertex {
  int deg;
  int nei[MD];
} *b;

uchar **a;

int P;

void addedge(int u, int v);

int main(int argc, char *argv[])
{
  int h,i,j,k,u,v;

  if(argc < 2){
    fprintf(stderr,"usage: %s N < edgelist\n",argv[0]);
    exit(0);
  }
  P = atoi(argv[1]);
  b = (struct vertex *)malloc(P * sizeof(struct vertex));
  a = (uchar **) malloc(P*sizeof(uchar *));
  for(i=0;i<P;i++)
    a[i] = (uchar *) malloc(P*sizeof(uchar));
  for(i=0;i<P;i++)
    for(j=0;j<P;j++)
      a[i][j] = 0;
  for(i=0;i<P;i++)
    b[i].deg = 0;
  while(scanf("%d %d",&u,&v) == 2){
    addedge(u,v);
    --u; --v;
    a[u][v]++;
    a[v][u]++;
  }
  for(i=0;i<P;i++){
    for(j=0;j<P;j++)
      printf("%1d",a[i][j]);
    printf("\n");
  }
}
void addedge(int u, int v)
{
  b[u].nei[DEG(u)++] = v;
  b[v].nei[DEG(v)++] = u;
}
