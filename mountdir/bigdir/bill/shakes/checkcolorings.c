
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <time.h>
#include <sys/resource.h>

/* MASK has NC 1-bits, NC = Number of Colors */

#define NC         4
#define MASK      15

#define NMAX     128
#define DMAX      96

#define DEG(X)         b[X].deg
#define RECOLOR(X,Y)   color[X] = Y

typedef
  unsigned char
uchar;

typedef struct vertex {
  int deg;
  int nbr[DMAX];
} VERTEX;

int N,D;

uchar cf[NMAX];       // moved out of VERTEX for memcpy; worth it?
uchar color[NMAX];    // ditto
int used[NC];

int same[NMAX][NMAX];

uchar a[NMAX][NMAX];  // adjacency matrix (needed?)
VERTEX b[NMAX];       // adjacency list

int sig[NMAX];

int oneleft[1 << NC];  // oneleft[i] >= 0 indicates the single feasible color for mask i

int maxcount;
int count = 0;
long calls = 0;        // dfs calls

void dfs(int v);
void vorder(int sig[]);
void setup();
void recolor(int u, int c);

int whoami;            // fake process id
FILE *logfd;           // log file descriptor

int main(int argc, char *argv[])
{
  int i,j,k,u,v,best;
  char filename[64];
  char fname[64];
  FILE *fd;
  int t0;

  logfd = stderr;
  whoami = 0;
  t0 = time(0);
  N = atoi(argv[1]);
  if(N > NMAX){
    fprintf(logfd,"N too large: %d > %d\n", N, NMAX);
    exit(0);
  }
  fd = fopen(argv[2],"r");
  if(!fd){
    fprintf(logfd,"cannot open %s\n", argv[2]);
    exit(0);
  }
  if(argc == 4)
    maxcount = atoi(argv[3]);
  else
    maxcount = 0x3fffffff;
  memset(a,0,NMAX*NMAX);
  memset(same,0,NMAX*NMAX*sizeof(int));
  for(i=0;i<N;i++)
    b[i].deg = 0;
  for(i=0;i<N;i++) 
    for(j=0;j<N;j++){
      fscanf(fd,"%1d",&k);
      a[i][j] = k;
      if(k)
        b[i].nbr[b[i].deg++] = j;
    }
  fclose(fd);

  for(i=0;i<N;i++) 
    for(j=0;j<N;j++)
      if(a[i][j] != a[j][i]){
        fprintf(stderr,"Asymmetry at (%d,%d).  N = %d?\n", i,j,N);
        exit(0);
      }
  setup();
  dfs(1);
  best = -1;
  for(i=0;i<N-1;i++)
    for(j=i+1;j<N;j++)
      if(same[i][j] > best){
        best = same[i][j]; u = i; v = j;
      }
  fprintf(logfd,"%9d%9d%9d%6d%6d %12.6lf  %s\n",
    count,best,count-best,u,v,(double) best/ (double) count,argv[2]);
}
void recolor(int u, int c)
{
  color[u] = c;
}
void setup()
{
  int i;

  for(i=0;i<(1 << NC);i++)
    oneleft[i] = -1;
  for(i=0;i<NC;i++)
    oneleft[MASK ^ (1 << i)] = i;
  vorder(sig);
  RECOLOR(0,0);
  for(i=1;i<N;i++)
    RECOLOR(i,NC);
  memset(cf,0,N);
  for(i=0;i<DEG(0);i++)
    cf[b[0].nbr[i]] |= (1 << 0);
  for(i=0;i<NC;i++)
    used[i] = 0;
  used[0] = 1;
}
void vorder(int sig[])
{
  uchar f[N];
  int i,j,k,m,t,v;

  memset(f,0,N);
  sig[0] = 0;
  f[0] = 1;
  for(t=1;t<N;t++){
    m = -1;
    for(i=1;i<N;i++){
      if(f[i])
        continue;
      k = 0;
      for(j=0;j<t;j++)
        if(a[i][sig[j]])
          k++;
      if(k > m || (k == m && DEG(i) > DEG(v))){
        m = k;
        v = i;
      }
    }
    sig[t] = v;
    f[v] = 1;
  }
#ifdef DEBUG
  for(i=0;i<N;i++)
    printf("%3d%6d\n",i,sig[i]);
#endif
}
void savestate(uchar scolor[], uchar scf[])
{
  memcpy(scf,cf,N);
  memcpy(scolor,color,N);
}
void restorestate(uchar scolor[], uchar scf[])
{
  memcpy(cf,scf,N);
  memcpy(color,scolor,N);
}
int colorvertex(int v, int c)
{
  int f,i,j,t,u;

  if(color[v] != NC){
    fprintf(logfd,"attempting to recolor %d from %d to %d\n",v,color[v],c);
    exit(0);
  }
  RECOLOR(v,c);                               // assign color c to v
  for(i=0;i<DEG(v);i++){                   
    u = b[v].nbr[i];                          // for each neighbor of v
    if(color[u] == c){                        // if it's color c, give up
      return 0;
    } else if(color[u] < NC){                 // if it's already colored, fine
      continue;
    }
    if(cf[u] & (1 << c))                      // if color c already forbidden for u, fine
      continue;
    cf[u] |= (1 << c);                        // forbid c for u
    if((color[u] == NC) && (cf[u] == MASK))   // if no colors are left, give up
      return 0;
    t = oneleft[cf[u]];                       // if only one color left, color it
    if(t >= 0){
      if(!colorvertex(u,t))                   // recursively check that color assignment
        return 0;
    }
  }
  return 1;
}
void dfs(int dep)
{
  int f,i,j,u,v,c;
  uchar scolor[N];
  uchar scf[N];

  if(dep == N){
    for(i=0;i<N-1;i++)
      for(j=i+1;j<N;j++)
        if(color[i] == color[j])
          same[i][j]++;
    count++;
    if(count >= maxcount) exit(0);
    return;
  }
  v = sig[dep];              // vertices ordered by # of lower numbered neighbors
  if(color[v] != NC){        // the color of this vertex was already forced
    dfs(dep+1);
    return;
  }
  for(c=0;c<NC;c++){
    if(color[v] != NC){
       fprintf(logfd,"error: vertex %d is not uncolored at step %d, depth %d\n",v,c,dep);
       exit(0);
    }
    if(cf[v] & (1 << c))      // c is a forbidden color
      continue;
    savestate(scolor,scf);    // save state: assigned colors and forbidden colors
    if(colorvertex(v,c)){
      used[c]++;
      dfs(dep+1);
      if(count >= maxcount) exit(0);
      used[c]--;
    }
    if(used[c] == 0) return;  // if more than one color is still unused, no need to try them all
    restorestate(scolor,scf);
  }
}

