
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <time.h>
#include <sys/resource.h>

#define SMAX             8

typedef
  unsigned char
uchar;

uchar asub[SMAX][SMAX];
int S;

void vorder();
void savegraph();
void checksize(int u);

int main(int argc, char *argv[])
{
  int f,i,j,k;
  int iter,gno,nedges;
  char filename[64];
  FILE *fd;

  fd = fopen("g08.data","r");
  if(!fd){
    fprintf(stderr,"cannot open glist\n");
    exit(0);
  }
  gno = 0;
  for(;;){
    gno++;
    if(fscanf(fd,"%d", &S) != 1) exit(0);
    nedges = 0;
    for(i=0;i<S;i++){
      for(j=0;j<S;j++){
        if(fscanf(fd,"%1d",&k) != 1){
          fprintf(stderr,"bad data at %d,%d,%d\n", gno,i,j);
          exit(0);
        }
        asub[i][j] = k;
        if(k) nedges++;
      }
    }
    fprintf(stderr,"graph %d has %d vertices and %d edges\n", gno, S, nedges/2);
    fprintf(stderr,"Before:\n");
    vorder();
    fprintf(stdout,"%d\n",S);
    savegraph(stdout);
  }
}
void vorder()
{
  int sig[SMAX];
  uchar tmp[SMAX][SMAX];
  uchar f[SMAX];
  int i,j,k,m,t,v;

  for(i=0;i<S;i++)
    sig[i] = i;
  memset(f,0,S);
  sig[0] = 0;
  f[0] = 1;
  for(t=1;t<S;t++){
    m = -1;
    for(i=1;i<S;i++){
      if(f[i])
        continue;
      k = 0;
      for(j=0;j<t;j++)
        if(asub[i][sig[j]])
          k++;
      if(k > m){
        m = k;
        v = i;
      }
    }
    sig[t] = v;
    f[v] = 1;
  }
  for(i=0;i<S;i++)
    for(j=0;j<S;j++)
      tmp[i][j] = asub[i][j];
  for(i=0;i<S;i++)
    for(j=0;j<S;j++)
      asub[i][j] = tmp[sig[i]][sig[j]];
}
void savegraph(FILE *fd)
{
  int i,j;

  for(i=0;i<S;i++){
    for(j=0;j<S;j++)
      fprintf(fd,"%1d",asub[i][j]);
    fprintf(fd,"\n");
  }
}
