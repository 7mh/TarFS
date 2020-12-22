
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <time.h>
#include <sys/resource.h>

#define RND(X)         (lrand48() % (X))
#define ADJACENT(X,Y)  (b[(X)][(Y)>>3] & (1 << ((Y) & 0x7)))
#define SETBIT(X,Y)     b[(X)][(Y)>>3] |= (1 << ((Y) & 0x7))

#define NN     91219

#define GFILE   "elist"

typedef
  unsigned char
uchar;

uchar *b[NN];
int deg[NN];

void readgraph();

int main(int argc, char *argv[])
{
  int i,j,k,m,n,u,v;
  uchar f[NN];
  int ct[NN];
  FILE *fd;
  char filename[64];

  n = (argc == 1) ? 0 : atoi(argv[1]);
  srand48(getpid());

  readgraph();
  memset(ct,0,NN*sizeof(int));
  for(i=0;i<NN;i++)
    ct[deg[i]]++;
  k = 0;
  for(i=0;i<NN;i++){
    if(ct[i] > 0)
      fprintf(stderr,"%3d%6d\n",i,ct[i]);
    if(i >= n)
      k += ct[i];
  }
  fprintf(stderr,"%d\n", k);
  m = 0;
  for(i=0;i<NN-1;i++){
    if(deg[i] < n) continue;
    for(j=i+1;j<NN;j++){
      if(deg[j] < n) continue;
      if(ADJACENT(i,j))
        m++;
    }
  }
  fprintf(stderr,"%d\n", m);
  for(i=0;i<NN;i++){
    if(deg[i] < n) continue;
    for(j=0;j<NN;j++){
      if(deg[j] < n) continue;
      printf("%1d", ADJACENT(i,j) ? 1 : 0);
    }
    printf("\n");
  }
}
void readgraph()
{
  FILE *fd;
  int i,j,k,u,v;

  fd = fopen(GFILE,"r");
  if(!fd){
    fprintf(stderr,"unable to open %s\n", GFILE);
    exit(0);
  }
  for(i=0;i<NN;i++){
    b[i] = malloc((NN+7)/8);
    memset(b[i],0,(NN+7)/8);
  }
  memset(deg,0,NN*sizeof(int));
  while(fscanf(fd,"%d %d", &u, &v) == 2){
    SETBIT(u,v);
    SETBIT(v,u);
    deg[u]++;
    deg[v]++;
  }
  fclose(fd);
}
