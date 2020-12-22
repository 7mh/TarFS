
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

typedef
  unsigned char
uchar;

#define RND(X)         (lrand48() % (X))
#define DEG(X)         (b[X].deg)
#define RECOLOR(X,Y)   color[X] = Y

#define EMAX        22

#define N            9
#define M           36
#define D           (N-1)
#define MAXNC        7
#define NS          11
#define NUDSUBS     14

#define SUBFILE  "august04.data"

typedef struct udsubgraphtype {
  int n;
  uchar a[NS][NS];
} UDSUBGRAPH;

typedef struct vertex {
  int deg;
  int nei[D];
} VERTEX;

int exhaustive();
void dfs(int v);
void xsetup();
void recolor(int u, int c);
int check_subgraphs();

UDSUBGRAPH sublist[NUDSUBS];

uchar a[N][N];
int elist[M][2];

int NC;
unsigned int MASK;

uchar cf[N];       // moved out of VERTEX for memcpy; worth it?
uchar color[N];    // ditto

int used[MAXNC];
int sig[N];
int oneleft[1 << MAXNC];  // oneleft[i] >= 0 indicates the single feasible color for mask i

VERTEX b[N];
int samecolor[N][N];

int globalflag;

int whoami;
FILE *logfd;
void readsubgraphs();
void shuffle(int e[M][2]);
void makelist(int e[M][2]);
void savegraph();
int makeAdjList();

int main(int argc, char *argv[])
{
  int f,i,j,k,m,u,v;
  int iter,emax,fmin;
  char filename[64];

  srand48((getpid() << 7) ^ time(0));
  whoami = atoi(argv[1]);
  if(whoami){
    fclose(stdin);
    fclose(stdout);
    fclose(stderr);
    if(fork())
      exit(0);
    sprintf(filename,"log.%03d",whoami);
    logfd = fopen(filename,"w");
    stdout = stderr = logfd;
    setbuf(logfd,0);
  } else {
    logfd = stderr;
  }
  emax = 0;
  fmin = 0x7fffff;
  setpriority(0, 0, 19);
  readsubgraphs();

  makelist(elist);
  for(iter=1;;iter++){
    memset(a,0,N*N);
    shuffle(elist);
    m = 0;
    for(i=0;i<M;i++){
      u = elist[i][0];
      v = elist[i][1];
      if(a[u][v]) continue;
      a[u][v] = a[v][u] = 1;
      if(!check_subgraphs()){
        m++;
        if(m == EMAX) break;
        continue;
      }
      a[u][v] = a[v][u] = 0;
    }
    f = exhaustive();
    if(f < 0)
      continue;
    if(f < fmin){
      fmin = f;
      fprintf(logfd,"%3d%9d%9d\n",whoami,iter,fmin);
      if(fmin == 0){
        savegraph();
        exit(0);
      }
    }
  }
}
void savegraph()
{
  int i,j;
  FILE *fd;
  char savefile[64];

  sprintf(savefile,"g.%02d.%03d", N, whoami);
  fd = fopen(savefile, "w");
  for(i=0;i<N;i++){
    for(j=0;j<N;j++)
      fprintf(fd,"%1d",a[i][j]);
    fprintf(fd,"\n");
  }
  fclose(fd);
}
int dfs_subgraph(int dep, uchar used[], int li[], UDSUBGRAPH *p)
{
  int i,j;
  uchar bad;

  if(dep == p->n)
    return 1;
  for(i=0;i<N;i++){
    if(used[i]) continue;
    bad = 0;
    for(j=0;j<dep;j++){
      if(p->a[j][dep] == 0) continue;
      if(!a[i][li[j]]){
        bad = 1;
        break;
      }
    }
    if(!bad){
      li[dep] = i;
      used[i] = 1;
      if(dfs_subgraph(dep+1,used,li,p))
        return 1;
      used[i] = 0;
    }
  }
  return 0;
}
int check_subgraphs()
{
  int i;
  int li[N];
  uchar used[N];

  for(i=0;i<NUDSUBS;i++){
    memset(used,0,N);
    if(dfs_subgraph(0,used,li,&sublist[i]))
      return 1;
  }
  return 0;
}
void readsubgraphs()
{
  FILE *fd;
  int i,j,k,x,y;

  fd = fopen(SUBFILE,"r");
  if(!fd){
    fprintf(stderr,"unable to open %s\n", SUBFILE);
    exit(0);
  }
  for(k=0;k<NUDSUBS;k++){
    if(fscanf(fd,"%d",&x) != 1){
      fprintf(stderr,"reading problem: k = %d\n",k);
      exit(0);
    }
    sublist[k].n = x;
    for(i=0;i<x;i++){
      for(j=0;j<x;j++){
        fscanf(fd,"%1d", &y);
        sublist[k].a[i][j] = y;
      }
    }
  }
  fclose(fd);
}

int makeAdjList()
{
  int i,j,k;

  k = 0;
  for(i=0;i<N;i++)
    b[i].deg = 0;
  for(i=0;i<N-1;i++)
    for(j=i+1;j<N;j++)
      if(a[i][j]){
        b[i].nei[b[i].deg++] = j;
        b[j].nei[b[j].deg++] = i;
        k++;
      }
  return k;
}
void addedge(int u, int v)
{
  b[u].nei[b[u].deg++] = v;
  b[v].nei[b[v].deg++] = u;
}
void deledge(int u, int v)
{
  b[u].deg--;
  b[v].deg--;
}
void recolor(int u, int c)
{
  color[u] = c;
}
void xsetup()
{
  int i;

  for(i=0;i<(1 << NC);i++)
    oneleft[i] = -1;
  for(i=0;i<NC;i++)
    oneleft[MASK ^ (1 << i)] = i;
  RECOLOR(0,0);
  for(i=1;i<N;i++)
    RECOLOR(i,NC);
  memset(cf,0,N);
  for(i=0;i<DEG(0);i++)
    cf[b[0].nei[i]] |= (1 << 0);
  for(i=0;i<NC;i++)
    used[i] = 0;
  used[0] = 1;
}
void savestate(uchar scolor[], uchar scf[], int n)
{
  memcpy(scf,cf,n);
  memcpy(scolor,color,n);
}
void restorestate(uchar scolor[], uchar scf[], int n)
{
  memcpy(cf,scf,n);
  memcpy(color,scolor,n);
}
int colorvertex(int v, int c)
{
  int f,i,j,t,u;

  if(color[v] != NC){
    fprintf(logfd,"attempting to recolor %d from %d to %d\n",v,color[v],c);
    exit(0);
  }
  RECOLOR(v,c);
  for(i=0;i<DEG(v);i++){
    u = b[v].nei[i];
    if(color[u] == c){
      return 0;
    } else if(color[u] < NC){
      continue;
    }
    if(cf[u] & (1 << c))                          // color c already forbidden for u
      continue;
    cf[u] |= (1 << c);                            // forbid c for u
    if((color[u] == NC) && (cf[u] == MASK))       // if no colors left, end of the line
      return 0;
    t = oneleft[cf[u]];                           // if only one color left, color it
    if(t >= 0){
      if(!colorvertex(u,t))
        return 0;
    }
  }
  return 1;
}
void dfs(int dep)
{
  int i,j,u,v,c;
  uchar scolor[N];
  uchar scf[N];

  if(dep == N){
    globalflag++;
    if(NC == 5)
      for(i=0;i<N-1;i++)
        for(j=i+1;j<N;j++)
          if(color[i] == color[j])
            samecolor[i][j]++;
    return;
  }
  v = dep;
  if(color[v] != NC){        // the color of this vertex was already forced
    dfs(dep+1);
    return;
  }
  for(c=0;c<NC;c++){
    if(color[v] != NC){
       fprintf(logfd,"vertex %d is not uncolored at step %d, depth %d\n",v,c,dep);
       exit(0);
    }
    if(cf[v] & (1 << c))      // can't use this color
      continue;
    savestate(scolor,scf,N);
    if(colorvertex(v,c)){
      used[c]++;
      dfs(dep+1);
      if(globalflag & (NC != 5)) return;
      used[c]--;
    }
    if(used[c] == 0) return;
    restorestate(scolor,scf,N);
  }
}
int exhaustive()
{
  time_t t;
  int flag = 0;
  int i,j,u,v,best,nedges;
  static int atbest = 0x7fffffff;

  nedges = makeAdjList();
  globalflag = 0;

  NC = 4;
  MASK = (1 << NC) - 1;
  xsetup();
  dfs(1);
  if(globalflag)
    return -1;

  fprintf(logfd,"%3d found a 5-chromatic graph!\n", whoami);

  return 0;

  memset(samecolor, 0, N*N*sizeof(int));
  NC = 5;
  MASK = (1 << NC) - 1;
  xsetup();
  dfs(1);
  if(!globalflag){
    fprintf(logfd,"%3d found 6-chromatic graph?\n", whoami);
    return 0;
  }
  best = 0x7fffffff;
  for(i=0;i<N-1;i++)
    for(j=i+1;j<N;j++)
      if(!a[i][j]){
        if(samecolor[i][j] == 0){
          fprintf(logfd,"pair: %d %d\n", i,j);
          return 0;
        }
        if(samecolor[i][j] < best){
          u = i; v = j;
          best = samecolor[i][j];
        }
      }
  return best;
}
void makelist(int e[M][2])
{
   int i,j,k;

   k=0;
   for(i=0;i<N-1;++i)
      for(j=i+1;j<N;++j){
         e[k][0]=i;
         e[k][1]=j;
         ++k;
      }
}
void shuffle(int e[M][2])
{
   int i,j,k,t;

   for(i=0;i<M;++i){
      j=RND(M);
      for(k=0;k<2;++k){
         t=e[i][k];
         e[i][k]=e[j][k];
         e[j][k]=t;
      }
   }
}
