
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define NMAX  256
#define LM     10
#define NF      5
#define NC     25

#define RND(X) (lrand48() % (X))
#define RNCOLOR (RND(nc))

typedef
  unsigned char
uchar;

int N;
int nc;
uchar a[NMAX][NMAX];

void goofy(int c[]);

int main(int argc, char *argv[])
{
  int d,i,j,k,u,v;
  int color[NMAX];
  char filename[64];
  FILE *fd;

  srand48(getpid());
  if(argc != 3){
    fprintf(stderr,"usage: %s N Matrix\n", argv[0]);
    exit(0);
  }
  N = atoi(argv[1]);
  fd = fopen(argv[2],"r");
  d = 0;
  for(i=0;i<N;i++)
    for(j=0;j<N;j++){
      fscanf(fd,"%1d", &k);
      a[i][j] = k;
      if(k) d++;
    }
  fclose(fd);
  for(i=0;i<N-1;i++)
    for(j=i+1;j<N;j++)
      if(a[i][j] != a[j][i]){
        fprintf(stderr,"inconsistency at %d-%d\n",i,j);
        exit(0);
      }
  nc = 4;
  colorgraph(color);
  fprintf(stderr,"Success with %d colors\n",nc);
  for(i=0;i<N;i++)
    printf("%d",color[i]+1);
  printf("\n");
}
int play(int c[NMAX], int n)
{
  int i,j,p[NMAX];
  int us[NMAX];

  for(i=0;i<nc;++i)
    us[i]=0;
  for(i=0;i<N-1;++i)
    for(j=i+1;j<N;++j)
      if(a[i][j] && (c[i]==c[j]))
        us[i]=us[j]=1;
  scramble(p);
  for(i=0;(i<n)&&(i<N);++i)
    if(us[p[i]])
      c[p[i]]=RNCOLOR;
  for(j=i;j<n;++j)
    c[RND(N)]=RNCOLOR;

}
int subs(int ct[], int c[])
{
  int i,j,color;

  for(i=0;i<nc;++i)
    ct[i]=0;
  for(i=0;i<N-1;++i)
    for(j=i+1;j<N;++j)
      if(a[i][j] && ((color=c[i])==c[j]))
        ++ct[color];
   for(i=j=0;i<nc;++i)
     j+=ct[i];
   return(j);
}
int vsubs(int v, int ct[], int c[])
{
  int i;

  for(i=0;i<nc;++i)
    ct[i]=0;
  for(i=0;i<N;++i)
    if(i!=v)
      if(a[v][i])
        ++ct[c[i]];
}
int pick(int ct[NC])
{
  int i,bc[NC],nm,d;
  int m;

  m=0x7fff;
  nm=0;
  for(i=0;i<nc;++i){
    if(ct[i] < m){
      m=ct[i];
      nm=1;
      bc[0]=i;
    }
    else if(ct[i]==m){
      bc[nm++]=i;
    }
  }
  if(nm==1)
    return(bc[0]);
  else
    return(bc[RND(nm)]);
}
int count(int c[NMAX], int ct[NMAX])
{
  int i;

  for(i=0;i<nc;++i)
    ct[i]=0;
  for(i=0;i<N;++i)
    ++ct[c[i]];
}
int scramble(int p[])           /* SCRAMBLE: makes a scrambled vertex list */
{
  int i,j,k;

  for(i=0;i<N;++i)
    p[i]=i;
  for(i=0;i<N;++i){
    j=RND(N);
    k=p[i]; p[i]=p[j]; p[j]=k;
  }
}
int rancolor(int c[])
{
  int i;

  for(i=0;i<N;++i)
    c[i]=RNCOLOR;
}
void goofy(int c[])   /* checks that a coloring is good */
{
  int i,j;

  for(i=0;i<N-1;++i)
    for(j=i+1;j<N;++j)
      if(a[i][j] && (c[i]==c[j])){
        fprintf(stderr,"Goofy!\n");
        exit(0);
      }
  return;
}
int colorgraph(int color[])
{
  int c,i,j,v,sum,limp,lbest,best,iter;
  int vct[NMAX];
  int ct[NC],p[NMAX];

  rancolor(color);
  count(color,vct);
  best=lbest=sum=subs(ct,color);
  limp=0;
  iter=0;
  while(best){
    scramble(p);
    for(i=0;i<N;++i){
      v=p[i];
      vsubs(v,ct,color);
      j=pick(ct);
      color[v]=j;
    }
    sum=subs(ct,color);
    if(sum < lbest){
      lbest=sum;
      if(sum < best){
        best=sum;
      }
      limp=0;
    } else {
      ++limp;
    }
    for(i=0;i<N;++i)
      if((color[i]<0)||(color[i]>=nc)){
        fprintf(stderr,"Whoops.\n");
        exit(0);
      }
    if(limp==LM){
      limp=0;
      lbest=0x7fff;
      play(color,NF);
    }
    iter++;
  }
  goofy(color);
}
