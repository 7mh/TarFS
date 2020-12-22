
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <time.h>
#include <sys/resource.h>

#define N             3099
#define SMAX            10

#define GFILE           "g3099"

#define CHUNK           16

typedef
  unsigned char
uchar;

typedef struct vertex {
  int deg;
  int size;
  int *nei;
} VERTEX;

uchar **a;
uchar asub[SMAX][SMAX];
VERTEX alist[N];
int S;

int whoami;
FILE *logfd;

void readbiggraph(char *fname);
int dfs_subgraph(int dep, uchar used[], int li[]);
void savegraph();
void checksize(int u);
void vorder(int x);

int main(int argc, char *argv[])
{
  int f,i,j,k,u,v;
  int iter,gno,nedges;
  int li[SMAX];
  uchar used[N];
  char filename[64];
  FILE *fd;

  whoami = (argc == 1) ? 0 : atoi(argv[1]);
  if(whoami){
    srand48((whoami << 3)  ^ (getpid() << 7) ^ time(0));
    fclose(stdin);
    fclose(stdout);
    fclose(stderr);
    if(fork())
      exit(0);
    sprintf(filename,"log.%04d",whoami);
    logfd = fopen(filename,"w");
    setbuf(logfd,0);
    setpriority(0, 0, 20);
  } else {
    logfd = stderr;
    srand48(getpid());
  }
  readbiggraph(GFILE);
  fd = fopen("h08.data","r");
  if(!fd){
    fprintf(logfd,"%3d cannot open glist\n", whoami);
    exit(0);
  }
  gno = 0;
  for(;;){
    gno++;
    if(fscanf(fd,"%d", &S) != 1) exit(0);
    fprintf(logfd,"Reading graph %d of order %d.\n", gno, S);
    nedges = 0;
    for(i=0;i<S;i++){
      for(j=0;j<S;j++){
        if(fscanf(fd,"%1d",&k) != 1){
          fprintf(logfd,"%3d bad data at %d,%d,%d\n", whoami,gno,i,j);
          exit(0);
        }
        asub[i][j] = k;
        if(k) nedges++;
      }
    }
    fprintf(logfd,"Graph %d has size %d\n",gno,nedges/2);
    for(u=0;u<S;u++){
      vorder(u);
      memset(used,0,N);
      li[0] = 0;
      used[0] = 1;
    }
    f = dfs_subgraph(1,used,li);
    fprintf(logfd,"%3d%6d%6d\n",whoami,gno,f);
  }
}
void vorder(int x)
{
  int sig[SMAX];
  uchar tmp[SMAX][SMAX];
  uchar f[SMAX];
  int i,j,k,m,t,v;

  for(i=0;i<S;i++)
    sig[i] = i;
  memset(f,0,S);
  sig[0] = x;
  f[x] = 1;
  for(t=1;t<S;t++){
    m = -1;
    for(i=0;i<S;i++){
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
void savegraph()
{
  int i,j;

  for(i=0;i<S;i++){
    for(j=0;j<S;j++)
      fprintf(logfd,"%1d",asub[i][j]);
    fprintf(logfd,"\n");
  }
}
void checksize(int u)
{
  if(alist[u].deg >= alist[u].size){
    alist[u].size *= 2;
    alist[u].nei = realloc(alist[u].nei, alist[u].size * sizeof(int));
  }
}
void readbiggraph(char *fname)
{
  FILE *fd;
  int i,j,k,u,v;

  fd = fopen(fname,"r");
  if(!fd){
    fprintf(logfd,"unable to open %s\n", fname);
    exit(0);
  }
  a = malloc(N*sizeof(uchar *));
  for(i=0;i<N;i++){
    a[i] = malloc(N*sizeof(uchar));
    memset(a[i],0,N);
  }
  for(i=0;i<N;i++){
    alist[i].deg  = 0;
    alist[i].size = CHUNK;
    alist[i].nei  = malloc(CHUNK * sizeof(int));
  }
  for(u=0;u<N;u++){
    for(v=0;v<N;v++){
      fscanf(fd,"%1d",&k);
      a[u][v] = k;
      if(k && (u < v)){
        checksize(u);
        checksize(v);
        alist[u].nei[alist[u].deg++] = v;
        alist[v].nei[alist[v].deg++] = u;
      }
    }
  }
  fclose(fd);
}
int dfs_subgraph(int dep, uchar used[], int li[])
{
  int h,i,j,k;
  int ibase,ubase;
  uchar bad;

  if(dep == S){
    for(i=0;i<S;i++)
      fprintf(logfd,"%8d",li[i]);
    fprintf(logfd,"\n");
    for(i=0;i<S;i++){
      for(j=0;j<S;j++)
        fprintf(logfd,"%1d",a[li[i]][li[j]]);
      fprintf(logfd,"   ");
      for(j=0;j<S;j++)
        fprintf(logfd,"%1d",asub[i][j]);
      fprintf(logfd,"\n");
    }
    return 1;
  }
  ubase = ibase = -1;
  for(i=0;i<dep;i++)
    if(asub[i][dep]){
      ibase = i;
      ubase = li[i];
      break;
    }
  if(ubase < 0){
    fprintf(logfd,"bad vertex at depth %d\n",dep); exit(0);
    for(i=0;i<N;i++){
      if(used[i]) continue;
      li[dep] = i;
      used[i] = 1;
      if(dfs_subgraph(dep+1,used,li))
        return 1;
      used[i] = 0;
    }
  } else {
    for(h=0;h<alist[ubase].deg;h++){
      i = alist[ubase].nei[h];
      if(used[i]) continue;
      bad = 0;
      for(j=ibase+1;j<dep;j++){
        if(asub[j][dep] == 0) continue;
        if(a[li[j]][i] == 0){
          bad = 1;
          break;
        }
      }
      if(!bad){
        li[dep] = i;
        used[i] = 1;
        if(dfs_subgraph(dep+1,used,li))
          return 1;
        used[i] = 0;
      }
    }
  }
  return 0;
}
