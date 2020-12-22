
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <time.h>
#include <sys/resource.h>

#define V              220
#define N           200000

#define VFILE     "vectors.220"

#define BITS            64

typedef
  unsigned char
uchar;

long vec[V][3];
long pts[N][3];
int npts;

int whoami;
FILE *logfd;

void readvectors();
void expand(int n);
int newpoint(int n);
int adjacent(int u, int v);
unsigned long isqrt(unsigned long x);

int main(int argc, char *argv[])
{
  int i,j,k,n;
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
  readvectors();
  for(i=0;i<6;i++)
    pts[0][i] = 0;
  npts = 1;
  expand(0);
  n = npts;
  fprintf(logfd,"%3d%9d%12d\n", whoami, npts);
  for(i=1;i<n;i++)
    expand(i);
  fprintf(logfd,"%3d%9d%12d\n", whoami, npts);
  exit(0);
  k = 0;
  for(i=0;i<npts-1;i++)
    for(j=i+1;j<npts;j++)
      if(adjacent(i,j)){
        k++;
        fprintf(logfd,"%5d%7d\n",i,j);
      }
}
void savegraph()
{
  int i,j;

  for(i=0;i<N;i++){
    for(j=0;j<3;j++)
      fprintf(logfd,"%25.17lf",pts[i][j]);
    fprintf(logfd,"\n");
  }
}
void norm(long d[], long v[])
{
  long coeff[]={2L,23L,1L,46L,1L,46L};
  int i;

  d[0] = 0L;
  for(i=0;i<6;i++)
    d[0] += coeff[i] * v[i] * v[i];
  d[1] = 0L;
  for(i=0;i<6;i+=2)
    d[1] += 2*v[i]*v[i+1];
}
void readvectors()
{
  FILE *fd;
  long t, d[2];
  int i,j,k;

  fd = fopen(VFILE,"r");
  if(!fd){
    fprintf(logfd,"unable to open %s\n", VFILE);
    exit(0);
  }
  for(i=0;i<V;i++){
    for(j=0;j<6;j++){
      fscanf(fd,"%ld",&t);
      vec[i][j] = t;
    }
    norm(d,vec[i]);
    if(d[1] != 0){
      fprintf(stderr,"WTF for i == %d\n", i);
    } else {
      t = isqrt(d[0]);
      if(t*t == d[0])
        fprintf(stderr,"%3d%16ld\n",i,t);
      else
        fprintf(stderr,"%3d%24ld%16ld%16ld%24ld\n",i,d[0],d[1],t,t*t);
    }
  }
  fclose(fd);
}
int newpoint(int n)
{
  int f,i,j;

  for(i=0;i<n;i++){
    f = 0;
    for(j=0;j<6;j++){
      if(pts[n][j] != pts[i][j]){
        f = 1;
        break;
      }
    }
    if(!f)
      return 0;
  }
  return 1;
}
void expand(int n)
{
  int i,j,k;

  for(i=0;i<V;i++){
    for(j=0;j<6;j++)
      pts[npts][j] = pts[n][j] + vec[i][j];
    if(newpoint(npts))
      npts++;
    for(j=0;j<6;j++)
      pts[npts][j] = pts[n][j] - vec[i][j];
    if(newpoint(npts))
      npts++;
  }
}
int adjacent(int u, int v)
{
  int i,j,k;

  return 0;
}
unsigned long isqrt(unsigned long x)
{
  unsigned long e,r,t;

  if(x == 0)
    return x;
  
  e = (1L << (BITS-2));
  while(e > x)
    e >>= 2;
  r = 0;
  while(e){
    t = r + e;
    if(x >= t){
      x -= t;
      r += (e << 1);
    }
    r >>= 1;
    e >>= 2;
  }
  return r;
}

