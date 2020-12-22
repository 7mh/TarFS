
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <time.h>
#include <math.h>

#define MAXLEVEL     2

#define K         1024

#define LOCAL       30

#define DIM          8
#define DIST         4

#define R           60
#define R2        3600

int radical[]={1, 3, 11, 33, 35, 105, 385, 1155};

typedef
  unsigned char
uchar;

typedef struct vertex {
  long x[DIM];
  long y[DIM];
} VERTEX;

typedef struct bstnode {
  int n;
  struct bstnode *left, *right;
} BST;

VERTEX *vert;
int vsize;

BST *root = 0;

VERTEX vlist[LOCAL];
double sqlist[LOCAL];

int nverts;
int nedges;

BST *makenode(int k);
int isnewvertex(VERTEX *v);
void printvertex(FILE *fd, int n);
void readdeltas();
void addneighborhood(int w);
void length(long d[DIM], VERTEX *u);
void difference(VERTEX *w, VERTEX *u, VERTEX *v);
void distance(long d[DIM], VERTEX *u, VERTEX *v);
void expandvert();
int okdist(long d[]);
double flength(VERTEX *u);

int main(int argc, char *argv[])
{
  long d[DIST];
  int t0;
  int h,i,j,k,l,m,n,r,t,u,v,lim1,lim2,level;
  int *nearlist;
  FILE *fd;
  char fname[64];

  for(i=0;i<DIM;i++)
    sqlist[i] = sqrt((double) radical[i]);

  t0 = time(0);

  readdeltas(); 

  vsize = K;
  vert = malloc(vsize * sizeof(VERTEX));
  memset(vert, 0, sizeof(VERTEX));

  nverts = 1;
  root = makenode(0);
  fprintf(stderr,"%d level %d vertices\n", 0, nverts);

  lim1 = 0;
  lim2 = 1;
  for(level=1;level<=MAXLEVEL;level++){
    for(i=lim1;i<lim2;i++)
      addneighborhood(i);
    fprintf(stderr,"%d level %d vertices\n", level, nverts);
    lim1 = lim2;
    lim2 = nverts;
  }
  n = nverts;
  nearlist = malloc(nverts * sizeof(int));
  k = 0;
  for(i=0;i<nverts;i++)
    nearlist[k++] = i;
  fprintf(stderr,"%d near vertices\n", k);
  sprintf(fname,"vlist%d",MAXLEVEL);
  fd = fopen(fname,"w");
  for(i=0;i<k;i++){
    j = nearlist[i];
    printvertex(fd,j);
  }
  fclose(fd);
  fprintf(stderr,"done with vertices at time %d\n",time(0)-t0);
  nedges = 0;
  sprintf(fname,"elist%d",MAXLEVEL);
  fd = fopen(fname,"w");
  for(i=0;i<k-1;i++){
    u = nearlist[i];
    for(j=i+1;j<k;j++){
      v = nearlist[j];
      distance(d, &vert[u], &vert[v]);
#ifdef PRINT_DISTANCES
      for(h=0;h<DIM;h++)
        fprintf(stdout,"%15ld",d[h]);
      fprintf(stdout,"\n");
#endif
      if(okdist(d)){
        nedges++;
        fprintf(fd,"%d %d\n",i,j);
      }
    }
  }
  fclose(fd);
  fprintf(stderr,"found %d edges in %d seconds\n", nedges, time(0)-t0);
}
int okdist(long d[])
{
  int i;

  if(d[0] != R2) return 0;
  for(i=1;i<DIST;i++)
    if(d[i]) return 0;
  return 1;
}
void printvertex(FILE *fd, int n)
{
  int i;

  for(i=0;i<DIM;i++)
    fprintf(fd," %ld", vert[n].x[i]);
  for(i=0;i<DIM;i++)
    fprintf(fd," %ld", vert[n].y[i]);
  fprintf(fd,"\n");
}
void readdeltas()
{
  int i,j,k;
  FILE *fd;

  fd = fopen("gens","r");
  if(!fd) exit(0);
  for(i=0;i<LOCAL;i++){
    for(j=0;j<DIM;j++){
      fscanf(fd,"%d",&k);
      vlist[i].x[j] = k;
    }
    for(j=0;j<DIM;j++){
      fscanf(fd,"%d",&k);
      vlist[i].y[j] = k;
    }
  }
  fclose(fd);
}
int compare2vertex(int k, VERTEX *v)
{
  int i;

  for(i=0;i<DIM;i++){
    if(v->x[i] != vert[k].x[i])
      return (v->x[i] - vert[k].x[i]);
    if(v->y[i] != vert[k].y[i])
      return (v->y[i] - vert[k].y[i]);
  }
  return 0;
}
int isnewvertex(VERTEX *v)
{
  int cv;
  BST *tree;

  tree = root;
  for(;;){
    cv = compare2vertex(tree->n,v);
    if(cv == 0)
      return 0;
    if(cv < 0){
      if(tree->left){
        tree = tree->left;
      } else {
        tree->left = makenode(nverts);
        return 1;
      }
    } else {
      if(tree->right){
        tree = tree->right;
      } else {
        tree->right = makenode(nverts);
        return 1;
      }
    }
  }
}
BST *makenode(int k)
{
  BST *tmp;

  tmp = malloc(sizeof(BST));
  tmp->n = k;
  tmp->left = tmp->right = 0;
  return tmp;
}
void expandvert()
{
  vsize <<= 1;
  vert = realloc(vert, vsize * sizeof(VERTEX));
  fprintf(stderr,"vsize increased to %d\n", vsize);
}
void addneighborhood(int w)
{
  int i,j,k,m,n;

  for(i=0;i<LOCAL;i++){
    for(j=0;j<DIM;j++){
      vert[nverts].x[j] = vert[w].x[j] + vlist[i].x[j];
      vert[nverts].y[j] = vert[w].y[j] + vlist[i].y[j];
    }
    if(!isnewvertex(&vert[nverts]))
      continue;
    nverts++;
    if(nverts == vsize)
      expandvert();
  }
}
void length(long d[DIM], VERTEX *u)
{
  int i,j;
  int dx2[DIM], dy2[DIM];

  memset(dx2, 0, DIM * sizeof(int));
  memset(dy2, 0, DIM * sizeof(int));
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
void distance(long d[DIST], VERTEX *u, VERTEX *v)
{
  VERTEX tmp;

  difference(&tmp, u, v);
  length(d, &tmp);
}
