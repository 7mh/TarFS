
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <time.h>
#include <sys/resource.h>

#define ADJACENT(X,Y)  (b[(X)][(Y)>>3] & (1 << ((Y) & 0x7)))
#define SETBIT(X,Y)     b[(X)][(Y)>>3] |= (1 << ((Y) & 0x7))

#define NN     78853
#define LM     25000
#define NF        (1+RND(2))
#define LIMPMAX 5000

#define NC         3
#define N         37

#define TARGET   111

#define Z        300

#define ZMAX    5000
#define NT        50

#define LMM     1000
#define MF         5

#define MAXITER 100000

#define RND(X) (lrand48() % (X))
#define RNCOLOR (RND(NC))

typedef
  unsigned char
uchar;

uchar *b[NN];
uchar a[N][N];

void randsub(int li[], uchar f[]);
int ecount(int li[]);
int maxpick(int ct[], int n);
void readgraph();
void savegraph(int li[], int m);

int keyval;

int whoami;
FILE *logfd;

int main(int argc, char *argv[])
{
  int i,j,k,m,n,u,v;
  int old,flag,best,lbest,nedges,limp,iter,atlimp,gno=1;
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

  for(i=0;i<NN;i++)
    b[i] = malloc((NN+7)/8);
  readgraph();
  randsub(li,f);
  best = lbest = ecount(li);
  limp = 0;
  atlimp = 0;
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
      atlimp = 0;
      if(nedges > best){
        best = nedges;
        if(best >= TARGET)
          savegraph(li,best);
        fprintf(logfd,"%3d%6d%9d%9d%12d\n", whoami, N, gno, best, keyval);
      }
    } else {
      limp++;
      atlimp++;
      if(nedges < lbest){
        li[k] = old;
        f[u] = 0;
        f[old] = 1;
      }
      if(limp >= LMM){
        if(atlimp >= LIMPMAX){
          readgraph();
          randsub(li,f);
          atlimp = 0;
          gno++;
        } else {
          for(i=0;i<NF;i++){
            k = RND(N);
            do v = RND(NN); while(f[v]);
            f[li[k]] = 0;
            f[v] = 1;
            li[k] = v;
          }
        }
        limp = 0;
        lbest = ecount(li);
      }
    }
  }
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
  int ct[NT];
  int vli[NT];
  int i,j,k,t,u,v;

  memset(f,0,NN);
  for(i=0;i<N;i++){
    for(j=0;j<NT;j++){
      do u = RND(NN); while(f[u]);
      vli[j] = u;
      for(k=t=0;k<i;k++)
        if(ADJACENT(li[k],u))
          t++;
      ct[j] = t;
    }
    k = maxpick(ct,NT);
    li[i] = vli[k];
    f[vli[k]] = 1;
  }
  for(i=0;i<N;i++){
    u = li[i];
    for(j=0;j<N;j++){
      v = li[j];
      a[i][j] = ADJACENT(u,v) ? 1 : 0;
    }
  }
}
void readgraph()
{
  FILE *fd;
  int i,j,k,u,v;
  char GFILE[32];

  keyval = RND(101);
  sprintf(GFILE,"Plane/elist.%03d", keyval);
  fd = fopen(GFILE,"r");
  if(!fd){
    fprintf(logfd,"unable to open %s\n", GFILE);
    exit(0);
  }
  for(i=0;i<NN;i++)
    memset(b[i],0,(NN+7)/8);
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
