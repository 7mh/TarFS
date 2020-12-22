
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <time.h>
#include <sys/resource.h>

#define N               12
#define S                8
#define D               11

#define AFILE         "g.icos"

#define MAXITER    1000000

#define LM            1000
#define NF              (1+(RND(S)))

#define RND(X) (lrand48() % (X))
#define RNCOLOR (RND(nc))

typedef
  unsigned char
uchar;

typedef struct vertex {
  int deg;
  int nbr[D];
} VERTEX;

VERTEX alist[S];

uchar a[N][N];
uchar asub[S][S];

int tau[N];

int whoami;
FILE *logfd;

void readbiggraph(uchar b[N][N], char *fname);
void readsmallgraph(uchar b[S][S], char *fname);
int pick(int ct[]);
int containsgraph(int n);

int main(int argc, char *argv[])
{
  int d,i,j,k;
  int iter,gout;
  char filename[64];

  whoami = atoi(argv[1]);
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
    setpriority(0, 0, 20);
  } else {
    logfd = stderr;
    srand48(getpid());
  }
  readbiggraph(a,AFILE);
  for(i=0;i<N-1;i++)
    for(j=i+1;j<N;j++)
      if(a[i][j] != a[j][i]){
        fprintf(logfd,"asymmetry at %d-%d\n",i,j);
        exit(0);
      }
  for(iter=1;;iter++){
    memset(asub,0,S*S);
    for(i=0;i<S;i++)
      for(j=0;j<S;j++){
        if(fscanf(stdin,"%1d",&k) != 1) exit(0);
        asub[i][j] = k;
      }
    if(!containsgraph(S)){
      for(i=0;i<S;i++){
        for(j=0;j<S;j++)
          printf("%1d",a[i][j]);
        printf("\n");
      }
      fprintf(logfd,"%3d is good\n",iter);
    } else {
      fprintf(logfd,"%3d is BAD\n",iter);
    }
  }
}
void readbiggraph(uchar b[N][N], char *fname)
{
  FILE *fd;
  int i,j,k;

  fd = fopen(fname,"r");
  if(!fd){
    fprintf(logfd,"unable to open %s\n", fname);
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
int pick(int ct[])
{
  int i,bc[N],m,n,d;

  m = 0x7fff;
  n = 0;
  for(i=0;i<N;++i){
    if(ct[i] < m){
      m = ct[i];
      n = 1;
      bc[0] = i;
    } else if(ct[i] == m){
      bc[n++] = i;
    }
  }
  if(n==1)
    return(bc[0]);
  else
    return(bc[RND(n)]);
}
int vscore(int w, int sig[])
{
  int i,j,k,t,u,v;

  t = 0;
  u = sig[w];
  for(j=0;j<alist[w].deg;j++){
    k = alist[w].nbr[j];
    v = sig[k];
    if(!a[u][v])
      t++;
  }
  return t;
}
int score(int sig[])
{
  int i,j,k,t,u,v;

  t = 0;
  for(i=0;i<S;i++){
    u = sig[i];
    for(j=0;j<alist[i].deg;j++){
      k = alist[i].nbr[j];
      v = sig[k];
      if(u > v) continue;
      if(!a[u][v])
        t++;
    }
  }
  return t;
}
int containsgraph(int n)
{
  uchar used[N];
  int sig[N];
  int ct[N];
  int i,j,k,u,v;
  int nf,sum,lbest,best,limp,iter;

  for(i=0;i<S;i++){
    alist[i].deg = 0;
    for(j=0;j<S;j++){
      if(i == j) continue;
      if(asub[i][j])
        alist[i].nbr[alist[i].deg++] = j;
    }
  }
  for(i=0;i<N;i++)
    tau[i] = i;
  for(i=0;i<N;i++){
    j = RND(N);
    k = tau[i]; tau[i] = tau[j]; tau[j] = k;
  }
  memset(used,0,N);
  for(i=0;i<S;i++){
    sig[i] = tau[i];
    used[tau[i]] = 1;
  }
  limp = 0;
  best = lbest = score(sig);
  for(iter=1;iter<MAXITER;iter++){
    k = RND(S);
    u = sig[k];
    used[u] = 0;
    for(i=0;i<N;i++){
      if(used[i]){
        ct[i] = 0x7fffff;
        continue;
      }
      sig[k] = i;
      ct[i] = vscore(k,sig);
    }
    v = pick(ct);
    if(used[v]){
      fprintf(logfd,"WTF?\n");
      exit(0);
    }
    used[v] = 1;
    sig[k] = v;
    sum = score(sig);
    if(sum < lbest){
      lbest = sum;
      limp = 0;
      if(sum < best){
        best = sum;
//      fprintf(logfd,"%3d%12d%6d\n", whoami, iter, best);
        if(best == 0){
          return 1;
        }
      }
    } else {
      limp++;
      if(limp == LM){
        limp = 0;
        nf = NF;
        for(i=0;i<nf;i++){
          k = RND(S);
          used[sig[k]] = 0;
          do sig[k] = RND(N); while(used[sig[k]]);
          used[sig[k]] = 1;
        }
        lbest = score(sig);
      }
    }
  }
  return 0;
}
