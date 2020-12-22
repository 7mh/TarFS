
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <time.h>
#include <sys/resource.h>

/* MASK has NC 1-bits, NC = Number of Colors */

#define NC         5
#define MASK      31

#define NMAX     256
#define DMAX     128

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
int good;

uchar cf[NMAX];        // moved out of VERTEX for memcpy; worth it?
uchar color[NMAX];    // ditto
int used[NC];

uchar a[NMAX][NMAX];        // adjacency matrix (needed?)
uchar asave[NMAX][NMAX];   // adjacency matrix (needed?)
VERTEX b[NMAX];           // adjacency list

int sig[NMAX];

int oneleft[1 << NC];   // oneleft[i] >= 0 indicates the single feasible color for mask i
long calls = 0;        // dfs calls

void dfs(int v);
void vorder(int sig[]);
void setup();
void recolor(int u, int c);

int whoami;             // fake process id
FILE *logfd;           // log file descriptor

int main(int argc, char *argv[])
{
  int i,j,k,u,v;
  char filename[64];
  char fname[64];
  FILE *fd;
  int t0;

  whoami = atoi(argv[1]);
  if(whoami > 168) exit(0);
  if(whoami){
//
//   dissociate from terminal
//
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
  }
  t0 = time(0);
  N = 169;
  sprintf(fname,"g169");
  fd = fopen(fname,"r");
  if(!fd){
    fprintf(logfd,"cannot open %s\n", "whatever");
    exit(0);
  }
  memset(asave,0,NMAX*NMAX);
  for(i=0;i<N;i++) 
    for(j=0;j<N;j++){
      fscanf(fd,"%1d",&k);
      asave[i][j] = k;
    }
  fclose(fd);

  memset(a,0,NMAX*NMAX);
  for(i=0;i<N;i++)
    b[i].deg = 0;
  u = 0;
  for(i=0;i<N;i++){
    v = 0;
    for(j=0;j<N;j++){
      if(i != whoami && j != whoami){
        a[u][v] = asave[i][j];
        if(a[u][v])
          b[u].nbr[b[u].deg++] = v;
        v++;
      }
    }
    if(i != whoami)
      u++;
  }
  N = 168;
  setup();
  good = 0;
  dfs(1);
  fprintf(logfd,"%3d  found %d %d-colorings in %d seconds\n",whoami,good,NC,(int)time(0)-t0);
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
/*
    for(i=0;i<NC;i++){
      f = 0;
      for(j=0;j<N;j++)
        if(color[j] == i){
          fprintf(logfd,"%s%d",f ? "," : "  {",j+1);
          f = 1;
        }
      fprintf(logfd,"}\n");
    }
*/
    good++;
    if(good % 10000 == 1) fprintf(logfd,"%3d%12d\n", whoami, good);
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
      used[c]--;
    }
    if(used[c] == 0) return;  // if more than one color is still unused, no need to try them all
    restorestate(scolor,scf);
  }
}

