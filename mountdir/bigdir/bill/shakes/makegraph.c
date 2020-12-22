
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <time.h>
#include <math.h>

#define K         4096

#define N          214
#define S           23
#define DIM          4

#define R2         144

int nsofar, ctr=3;

typedef
  unsigned char
uchar;

char *methods[]={
  "original",
  "x-axis reflection",
  "y-axis reflection",
  "pi/3 rotation",
  "2*pi/3 rotation",
  "3*pi/3 rotation",
  "4*pi/3 rotation",
  "5*pi/3 rotation",
  "6*pi/3 rotation"
};

typedef struct vertex {
  int x[2];
  int y[2];
  int par;
  int how;
} VERTEX;

VERTEX info[N];

int pts[N][4];
uchar a[N][N];

int rad[DIM] = {3,11,1,33};

double frad[DIM];

int seed[S][4]={
 {0, 0, 0, 0},
 {0, 0, 0, -4},
 {0, 0, -6, -2},
 {0, 0, -6, 2},
 {-6, 0, 0, -2},
 {-4, 0, 0, 0},
 {-4, 0, -6, -2},
 {-4, 0, -6, 2},
 {-2, 0, 0, -2},
 {-2, 0, -6, -4},
 {-2, 0, -6, 4},
 {0, -6, -6, 0},
 {-5, -3, 3, 3},
 {-5, 3, -3, 3},
 {-2, -6, 0, 0},
 {-2, -6, 0, -4},
 {-2, 6, 0, 0},
 {-2, 6, 0, -4},
 {-6, -6, 0, 0},
 {-6, 6, 0, 0},
 {-4, 0, 0, -4},
 {0, 0, -12, 0},
 {-8, 0, 0, 0}
};

int dist(int u, int v);
void rotate(int u[4], int v[4]);
void rotations(int alpha, int omega);
void flips();
void copyto(int n, int v[], int ma, int h);
void printinfo(int n);

int main(int argc, char *argv[])
{
  int i,j,k,n;
  double x,y;

  for(i=0;i<4;i++)
    frad[i] = sqrt((double) rad[i]);

  for(i=0;i<S;i++){
    for(j=0;j<4;j++)
      pts[i][j] = seed[i][j];
    copyto(i,seed[i],-1,0);
  }
  nsofar = S;
  flips();

  n = 0;
  for(i=0;i<6;i++){
    k = nsofar;
    rotations(n, nsofar);
    if(nsofar == k)
      break;
    ctr++;
    n = k;
  }

  for(i=0;i<nsofar;i++){
    printinfo(i);
    continue;
    x = (double) pts[i][0] * frad[0] + (double) pts[i][1] * frad[1];
    y = (double) pts[i][2] * frad[2] + (double) pts[i][3] * frad[3];
    printf("%9.3f%9.3f\n",x,y);
    for(j=0;j<4;j++)
      printf("%5d",pts[i][j]);
    printf("\n");
  }
  exit(0);

  memset(a,0,N*N);
  for(i=0;i<N-1;i++){
    for(j=i+1;j<N;j++){
      k = dist(i,j);
      if(k)
        a[i][j] = a[j][i] = 1;
    }
  }
  for(i=0;i<N;i++){
    for(j=0;j<N;j++)
      printf("%1d",a[i][j]);
    printf("\n");
  }
}
void printinfo(int n)
{
  int j;

  printf("%3d:  [", n+1);
  for(j=0;j<2;j++)
    printf("%3d,",info[n].x[j]);
  for(j=0;j<2;j++)
    printf("%3d%c",info[n].y[j], j ? ']' : ',');
  printf("%9d", info[n].par + 1);
  printf("     %s\n", methods[info[n].how]);
}
void copyto(int n, int v[], int ma, int h)
{
  info[n].x[0] = v[0];
  info[n].x[1] = v[1];
  info[n].y[0] = v[2];
  info[n].y[1] = v[3];
  info[n].par = ma;
  info[n].how = h;
}
int find(int key[])
{
  int f,i,j,k,n;

  for(i=0;i<nsofar;i++){
    f = 1;
    for(j=0;j<4;j++){
      if(key[j] != pts[i][j]){
        f = 0;
        break;
      }
    }
    if(f) return i;
  }
  return -1;
}
int length(int len[], int d[])
{
  int i;

  len[0] =  0;
  for(i=0;i<DIM;i++)
    len[0] += d[i]*d[i]*rad[i];
  len[1] = 2*(d[0]*d[1] + d[2]*d[3]);
}
int dist(int u, int v)
{
  int i,j,k;
  int len[2];
  int delta[DIM];

  for(i=0;i<DIM;i++)
    delta[i] = seed[u][i] - seed[v][i];
  length(len,delta);
//  fprintf(stderr,"%5d%6d\n",len[0],len[1]);
  return (len[1] == 0) && (len[0] == 144 || len[0] == 576);
}
void rotate(int u[4], int v[4])
{
  int a,b,c,d;

  a = v[0]; b = v[1]; c = v[2]; d = v[3];
  if((a + c) % 2){
    fprintf(stderr,"a + c odd: %d %d %d %d\n",a,b,c,d);
    exit(0);
  }
  u[0] = (a - c)/2; 
  if((b + d) % 2){
    fprintf(stderr,"b + d odd: %d %d %d %d\n",a,b,c,d);
    exit(0);
  }
  u[1] = (b - 3*d)/2;
  u[2] = (3*a + c)/2; 
  u[3] = (b + d)/2; 
}
void flips()
{
  int f,i,j,k,n;
  int v[4];

  n = nsofar;
  for(i=0;i<n;i++){
    for(j=0;j<4;j++)
      v[j] = pts[i][j];
    v[2] = -v[2];
    v[3] = -v[3];
    if(find(v) < 0){
      memcpy(pts[nsofar],v,4*sizeof(int));
      copyto(nsofar,v,i,1);
      nsofar++;
    }
  }
  fprintf(stderr,"flip 1: %d\n",nsofar);
  n = nsofar;
  for(i=0;i<n;i++){
    for(j=0;j<4;j++)
      v[j] = pts[i][j];
    v[0] = -v[0];
    v[1] = -v[1];
    if(find(v) < 0){
      memcpy(pts[nsofar],v,4*sizeof(int));
      copyto(nsofar,v,i,2);
      nsofar++;
    }
  }
  fprintf(stderr,"flip 2: %d\n",nsofar);
}
void rotations(int alpha, int omega)
{
  int f,i,j,k,n;
  int u[4],v[4];

  n = nsofar;
  for(i=alpha;i<omega;i++){
    for(j=0;j<4;j++)
      v[j] = pts[i][j];
    rotate(u,v);
    if(find(u) < 0){
      memcpy(pts[nsofar],u,4*sizeof(int));
      copyto(nsofar,u,i,ctr);
      nsofar++;
    }
  }
  fprintf(stderr,"rotate %d: %d\n", ctr, nsofar);
}
