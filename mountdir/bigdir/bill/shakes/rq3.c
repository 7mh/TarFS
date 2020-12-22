
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <time.h>
#include <sys/resource.h>

#define RND(X)  (lrand48() % (X))

/* MASK has NC 1-bits, NC = Number of Colors */

#define NC         5
#define MASK      31

#define NMAX     400
#define DMAX     100

#define NGOODC 32268
#define REMOVE    54

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

uchar starter[NGOODC][169];

uchar cf[NMAX];       // moved out of VERTEX for memcpy; worth it?
uchar color[NMAX];    // ditto
int used[NC];

uchar a[NMAX][NMAX];
uchar asave[NMAX][NMAX];
VERTEX b[NMAX];

int sig[NMAX];

int oneleft[1 << NC];  // oneleft[i] >= 0 indicates the single feasible color for mask i

long calls = 0;        // dfs calls
int good;

void dfs(int v);
void vorder(int sig[]);
void setup();
void recolor(int u, int c);
void reorder(int sig[]);
int bitcount(uchar x);
int colorvertex(int v, int c);
void makeset(int inset[]);
void savegraph();

int whoami;            // fake process id
FILE *logfd;           // log file descriptor
FILE *danfd = 0;
FILE *colfd;

int main(int argc, char *argv[])
{
  int i,j,k,t,u,v;
  char filename[64];
  char fname[64];
  FILE *fd;
  int t0,done,best;
  char ff[NGOODC];
  int inset[NMAX];

  whoami = (argc > 1) ? atoi(argv[1]) : 0;
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
  srand48(getpid());
  colfd = fopen("clist","r");
  for(i=0;i<NGOODC;i++){
    for(j=0;j<169;j++){
      fscanf(colfd,"%1d",&k);
      starter[i][j] = k;
    }
  }
  fclose(colfd);
  t0 = time(0);
  N = 350;
  sprintf(fname,"g350");
  fd = fopen(fname,"r");
  if(!fd){
    fprintf(logfd,"cannot open %s\n", "whatever");
    exit(0);
  }
  for(i=0;i<N;i++){
    for(j=0;j<N;j++){
      fscanf(fd,"%1d",&k);
      asave[i][j] = k;
    }
  }
  fclose(fd);

  setup();
  best = 0x7fff;
  for(;;){
    N = 350;
    memset(a,0,NMAX*NMAX);
    for(i=0;i<N;i++)
      b[i].deg = 0;
    makeset(inset);
    u = 0;
    for(i=0;i<N;i++){
      v = 0;
      for(j=0;j<N;j++){
        if(inset[i] && inset[j]){
          a[u][v] = asave[i][j];
          if(a[u][v])
            b[u].nbr[b[u].deg++] = v;
          v++;
        }
      }
      if(inset[i])
        u++;
    }

    N -= REMOVE;

    memset(ff,0,NGOODC);
    for(i=0;i<NGOODC;i++){
      memset(cf,0,NMAX);
      for(j=0;j<NMAX;j++)
        color[j] = NC;
      done = 0;
      for(j=0;j<169;j++){
        if(color[j] != NC){
          if(color[j] != starter[i][j]){
//          fprintf(logfd,"%3d%9d%9d  %d != %d contradiction\n",whoami,i,j, color[j], starter[i][j]);
            ff[i] = 1;
            done = 1;
            break;
          }
        } else if(!colorvertex(j,starter[i][j])){
//        fprintf(logfd,"%3d%9d%9d eliminated\n",whoami,i,j);
          ff[i] = 1;
          done = 1;
          break;
        }
      }
    }
    for(i=j=0;i<NGOODC;i++)
      if(!ff[i])
        j++;
    if(j >= best) continue;
    best = j;
    for(i=0;i<N+REMOVE;i++)
      if(!inset[i])
        fprintf(logfd,"%5d", i+1);
    fprintf(logfd,"\n");
    fprintf(logfd,"%3d%9d\n",whoami,best);
    if(j == 18) break;
  }
  savegraph();
}
void savegraph()
{
  int i,j;
  FILE *fd;
  char filename[64];

  sprintf(filename,"r%03d",whoami);
  fd = fopen(filename, "w");
  for(i=0;i<N;i++){
    for(j=0;j<N;j++)
      fprintf(fd,"%1d",a[i][j]);
    fprintf(fd,"\n");
  }
  fclose(fd);
}

void makeset(int inset[])
{
  int i,j;

  for(i=0;i<N;i++)
    inset[i] = 1;
  for(i=0;i<REMOVE;i++){
    do j = 169 + RND(180); while(!inset[j] || (j == 199));
    inset[j] = 0;
  }
}
int bitcount(uchar x)
{
  int i;

  for(i=0;x;i++)
    x &= (x-1);
  return i;
}
void reorder(int sig[])
{
  int i,j,k,bc,bd;

  for(i=169;i<N-1;i++){
    k = i;
    bc = bitcount(cf[sig[i]]);
    for(j=i+1;j<N;j++){
      bd = bitcount(cf[sig[j]]);
      if(bd > bc){
        k = j;
        bc = bd;
      }
    }
    if(k != i){
      sig[i] ^= sig[k];
      sig[k] ^= sig[i];
      sig[i] ^= sig[k];
    }
  }
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
  for(i=0;i<N;i++)
    sig[i] = i;
  for(i=0;i<NC;i++)
    used[i] = 1;
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
  char filename[16];

  if(dep == N){
    good = 1;
    return;
    if(!danfd){
      sprintf(filename,"dan%03d.txt",whoami);
      danfd = fopen(filename,"w");
    }
    for(i=0;i<NC;i++){
      f = 0;
      for(j=0;j<N;j++)
        if(color[j] == i){
          fprintf(danfd,"%s%d",f ? "," : "  {",j+1);
          f = 1;
        }
      fprintf(danfd,"}\n");
    }
    for(i=0;i<N;i++)
      fprintf(logfd,"%d",color[i]);
    fprintf(logfd,"\n");
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
      if(good) return;
      used[c]--;
    }
    if(used[c] == 0) return;  // if more than one color is still unused, no need to try them all
    restorestate(scolor,scf);
  }
}

