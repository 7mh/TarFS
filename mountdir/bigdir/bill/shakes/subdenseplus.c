
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <time.h>
#include <sys/resource.h>

#define ADJACENT(X,Y)  (b[(X)][(Y)>>3] & (1 << ((Y) & 0x7)))
#define SETBIT(X,Y)     b[(X)][(Y)>>3] |= (1 << ((Y) & 0x7))

#define NN     63109
#define LM     10000
#define NF         1
#define NC         3
#define N         97

#define TARGET   415

#define Z        (25+whoami)

#define ZMAX    1024

#define LMM     1000
#define MF         5

#define MAXITER 100000

#define GFILE   "Plane/elist"

#define RND(X) (lrand48() % (X))
#define RNCOLOR (RND(NC))

typedef
  unsigned char
uchar;

uchar *b[NN];

void goofy(int c[]);
void randsub(int li[], uchar f[]);
int colorgraph();
int scramble(int p[]);
int ecount(int li[]);
int maxpick(int ct[], int n);
void readgraph();
void savegraph(int li[], int m);

int vlist[]={
38304,5369,4739,14518,18377,18981,10526,25247,5795,45306,5592,15350,1821,810,13483,3436,33119,486,6475,17752,1757,7761,11791,7107,40918,19669,24104,37684,2489,9959,16148,15077,43807,23969,13840,27115,1715,25502,786,21961,10027,323,7361,8170,11429,7933,2251,22375,4860,105,37666,3592,13480,384,1848,16545,2007,895,962,28143,40621,24062,13442,222,5623,8956,11204,25368,8527,18535,8402,15968,4068,10744,20908,41166,18364,24757,20006,26832,18114,3727,33252,2206,13213,9825,5982,5502,9311,139,9177
};

int whoami;
FILE *logfd;

int main(int argc, char *argv[])
{
  int i,j,k,m,n,u,v;
  int old,flag,best,lbest,nedges,limp,iter;
  int li[N];
  uchar f[NN];
  int can[ZMAX];
  int ct[ZMAX];
  FILE *fd;
  char filename[64];

  whoami = (argc == 1) ? 0 : atoi(argv[1]);
  if(whoami){
    srand48((whoami << 3)  ^ (getpid() << 7) ^ time(0));
    fclose(stdin);
    fclose(stdout);
    fclose(stderr);
    if(fork())
      exit(0);
    sprintf(filename,"log.%03d",whoami);
    logfd = fopen(filename,"w");
    setbuf(logfd,0);
    setpriority(0, 0, 19);
  } else {
    logfd = stderr;
    srand48(getpid());
  }

  readgraph();
  randsub(li,f);
  best = lbest = ecount(li);
  fprintf(logfd,"%3d%6d\n", whoami, best);
  limp = 0;
  for(;;){
    k = RND(N);
    can[0] = li[k];
    ct[0] = lbest;
    for(i=1;i<Z;i++){
      do j = RND(NN); while(f[j]);
      li[k] = j;
      can[i] = j;
      ct[i] = ecount(li);
    }
    j = maxpick(ct,Z);
    li[k] = can[j];
    f[can[0]] = 0;
    f[can[j]] = 1;
    nedges = ct[j];
    if(nedges > lbest){
      lbest = nedges;
      limp = 0;
      if(nedges > best){
        best = nedges;
        if(best >= TARGET)
          savegraph(li,best);
        fprintf(logfd,"%3d%6d%9d%9d\n", whoami, N, LM, best);
      }
    } else {
      limp++;
      if(nedges < lbest){
        li[k] = old;
        f[u] = 0;
        f[old] = 1;
      }
      if(limp >= LMM){
        for(i=0;i<NF;i++){
          k = RND(N);
          do v = RND(NN); while(f[v]);
          f[li[k]] = 0;
          f[v] = 1;
          li[k] = v;
        }
        limp = 0;
        lbest = ecount(li);
      }
    }
  }
}
int maxpick(int ct[], int n)
{
  int i,bc[n],nm,d;
  int m;

  m = -1;
  nm = 0;
  for(i=0;i<n;++i){
    if(ct[i] > m){
      m = ct[i];
      nm = 1;
      bc[0] = i;
    } else if(ct[i]==m){
      bc[nm++] = i;
    }
  }
  if(nm==1)
    return(bc[0]);
  else
    return(bc[RND(nm)]);
}
void randsub(int li[], uchar f[])
{
  int i,j,k,t,u,v;

  memset(f,0,NN);
  for(i=0;i<N-6;i++){
    u = vlist[i];
    li[i] = u;
    f[u] = 1;
  }
  for(i=N-6;i<N;i++){
    do u = RND(NN); while(f[u]);
    f[u] = 1;
    li[i] = u;
  }
}
void readgraph()
{
  FILE *fd;
  int i,j,k,u,v;

  fd = fopen(GFILE,"r");
  if(!fd){
    fprintf(logfd,"unable to open %s\n", GFILE);
    exit(0);
  }
  for(i=0;i<NN;i++){
    b[i] = malloc((NN+7)/8);
    memset(b[i],0,(NN+7)/8);
  }
  while(fscanf(fd,"%d %d", &u, &v) == 2){
    SETBIT(u,v);
    SETBIT(v,u);
  }
  fclose(fd);
}
void savegraph(int li[], int m)
{
  FILE *fd;
  char fname[64];
  int i,j,k;

  for(i=0;i<N;i++)
    fprintf(logfd,"%6d",li[i]);
  fprintf(logfd,"\n");
  
  sprintf(fname,"dense%03d.%03d.%03d",N,m,whoami);
  fd = fopen(fname,"w");
  if(!fd){
    fprintf(logfd,"unable to open %s\n", fname);
    exit(0);
  }
  for(i=0;i<N;i++){
    for(j=0;j<N;j++){
      fprintf(fd,"%1d",ADJACENT(li[i],li[j]) ? 1 : 0);
    }
    fprintf(fd,"\n");
  }
  fclose(fd);
}
int ecount(int li[])
{
  int i,j,k,u,v;

  k = 0;
  for(i=0;i<N-1;i++){
    u = li[i];
    for(j=i+1;j<N;j++){
      v = li[j];
      if(ADJACENT(u,v))
        k++;
    }
  }
  return k;
}

