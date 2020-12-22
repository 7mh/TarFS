
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <pthread.h>
#include <time.h>
#include <math.h>

#define RND(X)   (lrand48() % (X))

#define N     10000000
#define DIM          8

#define MAXLEN      10.00
#define EPSILON      0.01
#define K         1024

#define DMASK       63
#define VMASK       63

long radical[]={  1,  3,  5, 15, 11, 33, 55,165};

int limit[DIM];
double sq[DIM];

typedef
  unsigned char
uchar;

typedef struct vertex {
  long x[DIM];
  long y[DIM];
} VERTEX;

typedef struct bstnode {
  long dist[DIM];
  long count;
  struct bstnode *left, *right;
} DST;

typedef struct vernode {
  VERTEX *v;
  struct vernode *left, *right;
} VER;

DST *droot[DMASK+1];
VER *vroot[VMASK+1];

DST *dmakenode(long d[]);
VER *vmakenode(VERTEX *v);
int isnewdist(long d[]);
int isnewvert(VERTEX *v);
void printvertex(FILE *fd, VERTEX *v);
void addneighborhood(int w);
void length(long d[DIM], VERTEX *u);
void difference(VERTEX *w, VERTEX *u, VERTEX *v);
void distance(long d[DIM], VERTEX *u, VERTEX *v);
void traverse(DST *tree);
void randvertex(VERTEX *v);
double flength(VERTEX *u);
int goodlength(long dist[]);
void *worker(int *id);
void printdistance(FILE *fd, DST *t);
int dhash(long dist[]);
int vhash(VERTEX *v);
void largetweak(VERTEX *v);
void smalltweak(VERTEX *v);

long target[]={61,0,0,4,0,0,0,0};

int maxcount = 0;

FILE *distfd;
FILE *logfd;
int whoami;

#define NT    24

pthread_mutex_t mutdist[DMASK+1], mutvert[VMASK+1];

int id[NT];

int main(int argc, char *argv[])
{
  long dist[DIM];
  int t0;
  int i,j,k,l,m,n,u;
  double len;
  VERTEX *v;
  pthread_t th[NT];

  memset(vroot, 0, (VMASK + 1)*sizeof(VER *));
  memset(droot, 0, (DMASK + 1)*sizeof(DST *));

  for(i=0;i<NT;i++)
    id[i] = i;
  for(i=0;i<=DMASK;i++)
    pthread_mutex_init(&mutdist[i], 0);
  for(i=0;i<=VMASK;i++)
    pthread_mutex_init(&mutvert[i], 0);

  for(i=0;i<DIM;i++)
    sq[i] = sqrt((double) radical[i]);
  for(i=0;i<DIM;i++)
    limit[i] =  (sq[7] + EPSILON) / (int) sq[i];
  logfd = stderr;
  srand48(getpid());

  v = malloc(sizeof(VERTEX));
  memset(v,0,sizeof(VERTEX));
  memset(dist,0,DIM*sizeof(long));
  droot[dhash(dist)] = dmakenode(dist);
  vroot[vhash(v)] = vmakenode(v);

  for(i = 0 ; i < NT ; i++)
    pthread_create(&th[i], 0, (void *) worker, &id[i]);

  for(i = 0 ; i < NT ; i++)
    pthread_join(th[i], 0);

  distfd = fopen("d0.txt","w");
  for(i=0;i<=DMASK;i++)
    traverse(droot[i]);
  fclose(distfd);
}
void printvertex(FILE *fd, VERTEX *v)
{
  int i;

  for(i=0;i<DIM;i++)
    fprintf(fd,"%8ld",v->x[i]);
  for(i=0;i<DIM;i++)
    fprintf(fd,"%8ld",v->y[i]);
  fprintf(fd,"\n");
}
int goodlength(long dist[])
{
  int i;

  for(i=0;i<DIM;i++)
    if(dist[i] != target[i])
      return 0;
  return 1;
}
void printdistance(FILE *fd, DST *t)
{
  int i;

  fprintf(fd,"%6ld%6s",t->count," ");
  for(i=0;i<DIM;i++)
    fprintf(fd,"%10ld",t->dist[i]);
  fprintf(fd,"\n");
}
void traverse(DST *tree)
{
  if(!tree)
    return;
  if(tree->left)
    traverse(tree->left);
  if(tree->count > maxcount){
    maxcount = tree->count;
    printdistance(distfd, tree);
  }
  if(tree->right)
    traverse(tree->right);
}
int vhash(VERTEX *v)
{
  long hval;
  int i;

  hval = 0;
  for(i=0;i<DIM;i++){
    hval ^= (v->x[i] << i);
    hval ^= (v->y[i] << i);
  }
  return (int)(hval & VMASK);
}
int dhash(long dist[])
{
  long hval;
  int i;

  hval = 0;
  for(i=0;i<DIM;i++)
    hval ^= (dist[i] << i);
  return (int)(hval & DMASK);
}
int dcompare(long d1[], long d2[])
{
  int i;

  for(i=0;i<DIM;i++)
    if(d1[i] != d2[i])
      return (d1[i] - d2[i]);
  return 0;
}
int isnewdist(long d[])
{
  int hv;
  int cv;
  DST *tree;

  hv = dhash(d);
  pthread_mutex_lock(&mutdist[hv]);
  tree = droot[hv];
  if(!tree){
    droot[hv] = dmakenode(d);
    pthread_mutex_unlock(&mutdist[hv]);
    return 1;
  }
  for(;;){
    cv = dcompare(d,tree->dist);
    if(cv == 0){
      tree->count++;
      pthread_mutex_unlock(&mutdist[hv]);
      return 0;
    }
    if(cv < 0){
      if(tree->left){
        tree = tree->left;
      } else {
        tree->left = dmakenode(d);
        pthread_mutex_unlock(&mutdist[hv]);
        return 1;
      }
    } else {
      if(tree->right){
        tree = tree->right;
      } else {
        tree->right = dmakenode(d);
        pthread_mutex_unlock(&mutdist[hv]);
        return 1;
      }
    }
  }
}
int vcompare(VERTEX *u, VERTEX *v)
{
  return memcmp(u,v,sizeof(VERTEX));
}
int isnewvert(VERTEX *v)
{
  int hv;
  int cv;
  VER *tree;

  hv = vhash(v);
  pthread_mutex_lock(&mutvert[hv]);
  tree = vroot[hv];
  if(!tree){
    vroot[hv] = vmakenode(v);
    pthread_mutex_unlock(&mutvert[hv]);
    return 1;
  }
  for(;;){
    cv = vcompare(v,tree->v);
    if(cv == 0){
      pthread_mutex_unlock(&mutvert[hv]);
      return 0;
    }
    if(cv < 0){
      if(tree->left){
        tree = tree->left;
      } else {
        tree->left = vmakenode(v);
        pthread_mutex_unlock(&mutvert[hv]);
        return 1;
      }
    } else {
      if(tree->right){
        tree = tree->right;
      } else {
        tree->right = vmakenode(v);
        pthread_mutex_unlock(&mutvert[hv]);
        return 1;
      }
    }
  }
}
DST *dmakenode(long d[])
{
  int i;
  DST *tmp;

  tmp = malloc(sizeof(DST));
  for(i=0;i<DIM;i++)
    tmp->dist[i] = d[i];
  tmp->count = 1;
  tmp->left = tmp->right = 0;
  return tmp;
}
VER *vmakenode(VERTEX *v)
{
  int i;
  VER *tmp;

  tmp = malloc(sizeof(DST));
  tmp->v = malloc(sizeof(VERTEX));
  memcpy(tmp->v,v,sizeof(VERTEX));
  tmp->left = tmp->right = 0;
  return tmp;
}
void length(long d[DIM], VERTEX *u)
{
  int i,j;
  long dx2[DIM],dy2[DIM];

  memset(dx2, 0, DIM * sizeof(long));
  memset(dy2, 0, DIM * sizeof(long));
  for(i=0;i<DIM;i++){
    for(j=0;j<DIM;j++){
      dx2[i ^ j] += u->x[i] * u->x[j] * radical[i & j];
      dy2[i ^ j] += u->y[i] * u->y[j] * radical[i & j];
    }
  }
  for(i=0;i<DIM;i++)
    d[i] = dx2[i] + dy2[i];
}
void difference(VERTEX *w, VERTEX *u, VERTEX *v)
{
  int i;

  for(i=0;i<DIM;i++){
    w->x[i] = u->x[i] - v->x[i];
    w->y[i] = u->y[i] - v->y[i];
  }
}
void distance(long d[DIM], VERTEX *u, VERTEX *v)
{
  VERTEX tmp;

  difference(&tmp, u, v);
  length(d, &tmp);
}
void randvertex(VERTEX *v)
{
  int i;

  for(i=0;i<DIM;i++){
    v->x[i] = RND(limit[i]);
    if(RND(100) < 50) v->x[i] = -v->x[i];
    v->y[i] = RND(limit[i]);
    if(RND(100) < 50) v->y[i] = -v->y[i];
  }
}
double flength(VERTEX *u)
{
  int i;
  double tx,ty,rv;

  tx = ty = 0.0;
  for(i=0;i<DIM;i++){
    tx += ((double) u->x[i]) * sq[i];
    ty += ((double) u->y[i]) * sq[i];
  }
  return sqrt(tx*tx + ty*ty);
}
void largetweak(VERTEX *v)
{
  int i,j;

  for(i=0;i<2;i++){
    j = RND(DIM);
    v->x[j] = RND(limit[j]);
    if(RND(100) < 50) v->x[j] = -v->x[j];
    j = RND(DIM);
    v->x[j] = RND(limit[j]);
    if(RND(100) < 50) v->x[j] = -v->x[j];
  }
  i = ((lrand48() >> 2) & 0xf);
  if(i & 0x8)
    v->x[i & 7] = 0;
  else
    v->y[i & 7] = 0;
}
void smalltweak(VERTEX *v)
{
  int k;

  k = ((lrand48() >> 2) & 0x1f);
  if(k & 0x8)
    v->x[k & 7] += ((k & 0x10) ? 1 : -1);
  else
    v->y[k & 7] += ((k & 0x10) ? 1 : -1);
}
void *worker(int *me)
{
  int i,k;
  long dist[DIM];
  VERTEX *v;

  k = 0;
  v = malloc(sizeof(VERTEX));
  for(i=0;i<N;i++){
    randvertex(v);
    if(isnewvert(v)){
      length(dist,v);
      if(isnewdist(dist))
        k++;
    }
  }
  fprintf(logfd,"worker %2d found %9d\n", *me, k);
  pthread_exit(0);
}
