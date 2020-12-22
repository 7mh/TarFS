
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <time.h>
#include <math.h>

#define RND(X)   (lrand48() % (X))

#define N    100000000
#define DIM          8

#define MAXLEN      10.00
#define EPSILON      0.01
#define K         1024

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
} BST;

typedef struct vernode {
  VERTEX *v;
  struct vernode *left, *right;
} VER;

BST *root = 0;
VER *voot = 0;

BST *makenode(long d[]);
VER *makenode2(VERTEX *v);
int isnewdist(long d[]);
int isnewvert(VERTEX *v);
void printvertex(FILE *fd, VERTEX *v);
void addneighborhood(int w);
void length(long d[DIM], VERTEX *u);
void difference(VERTEX *w, VERTEX *u, VERTEX *v);
void distance(long d[DIM], VERTEX *u, VERTEX *v);
void traverse(BST *tree);
void randvertex(VERTEX *v);
double flength(VERTEX *u);
int goodlength(long dist[]);

long target[]={61,0,0,4,0,0,0,0};

int maxcount = 0;

FILE *distfd;
FILE *logfd;
int whoami;

int main(int argc, char *argv[])
{
  long dist[DIM];
  int t0;
  int i,j,k,l,m,n,u;
  double len;
  VERTEX *v;

  distfd = fopen("distances.txt","w");
  v = malloc(sizeof(VERTEX));
  for(i=0;i<DIM;i++)
    sq[i] = sqrt((double) radical[i]);
  for(i=0;i<DIM;i++)
    limit[i] =  (sq[7] + EPSILON) / (int) sq[i];
  logfd = stderr;
  srand48(getpid());
  memset(dist,0,DIM*sizeof(long));
  memset(v,0,sizeof(VERTEX));
  root = makenode(dist);
  voot = makenode2(v);
  k = 0;
  for(i=0;i<N;i++){
    randvertex(v);
    len = flength(v);
    if(isnewvert(v)){
      length(dist,v);
      if(isnewdist(dist))
        k++;
    }
  }
  fprintf(logfd,"%d distances\n",k);
  traverse(root);
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
void printdistance(BST *t)
{
  int i;

  fprintf(distfd,"%6ld%6s",t->count," ");
  for(i=0;i<DIM;i++)
    fprintf(distfd,"%10ld",t->dist[i]);
  fprintf(distfd,"\n");
}
void traverse(BST *tree)
{
  if(!tree)
    return;
  if(tree->left)
    traverse(tree->left);
  if(tree->count > maxcount){
    maxcount = tree->count;
    printdistance(tree);
  }
  if(tree->right)
    traverse(tree->right);
}
int compare(long d1[], long d2[])
{
  int i;

  for(i=0;i<DIM;i++)
    if(d1[i] != d2[i])
      return (d1[i] - d2[i]);
  return 0;
}
int isnewdist(long d[])
{
  int cv;
  BST *tree;

  tree = root;
  for(;;){
    cv = compare(d,tree->dist);
    if(cv == 0){
      tree->count++;
      return 0;
    }
    if(cv < 0){
      if(tree->left){
        tree = tree->left;
      } else {
        tree->left = makenode(d);
        return 1;
      }
    } else {
      if(tree->right){
        tree = tree->right;
      } else {
        tree->right = makenode(d);
        return 1;
      }
    }
  }
}
int compare2(VERTEX *u, VERTEX *v)
{
  return memcmp(u,v,sizeof(VERTEX));
}
int isnewvert(VERTEX *v)
{
  int cv;
  VER *tree;

  tree = voot;
  for(;;){
    cv = compare2(v,tree->v);
    if(cv == 0)
      return 0;
    if(cv < 0){
      if(tree->left){
        tree = tree->left;
      } else {
        tree->left = makenode2(v);
        return 1;
      }
    } else {
      if(tree->right){
        tree = tree->right;
      } else {
        tree->right = makenode2(v);
        return 1;
      }
    }
  }
}
BST *makenode(long d[])
{
  int i;
  BST *tmp;

  tmp = malloc(sizeof(BST));
  for(i=0;i<DIM;i++)
    tmp->dist[i] = d[i];
  tmp->count = 1;
  tmp->left = tmp->right = 0;
  return tmp;
}
VER *makenode2(VERTEX *v)
{
  int i;
  VER *tmp;

  tmp = malloc(sizeof(BST));
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
    if(RND(100) < 5) v->x[i] = -v->x[i];
    v->y[i] = RND(limit[i]);
    if(RND(100) < 5) v->y[i] = -v->y[i];
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

