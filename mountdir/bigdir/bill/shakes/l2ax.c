#include <stdio.h>
#include <stdlib.h>

#define RND(X) (random()%(X))
#define DEG(X) b[X].deg

#define MP 8192
#define MD  128
#define TG   30

typedef
  unsigned char
uchar;

struct vertex {
  int deg;
  int nei[MD];
} *b;

uchar **a;

int P,D;

int main(int argc, char *argv[])
{
  int h,i,j,k;

  if(argc != 3){
    fprintf(stderr,"usage: %s N D < alist\n",argv[0]);
    exit(0);
  }
  P = atoi(argv[1]);
  D = atoi(argv[2]);
  b = (struct vertex *)malloc(P * sizeof(struct vertex));
  a = (uchar **) malloc(P*sizeof(uchar *));
  for(i=0;i<P;i++)
    a[i] = (uchar *) malloc(P*sizeof(uchar));
  for(i=0;i<P;i++)
    for(j=0;j<P;j++)
      a[i][j] = 0;
  for(i=0;i<P;i++){
    for(j=0;j<D;j++){
      if(!scanf("%d",&k)){
        fprintf(stderr,"error reading\n");
        exit(0);
      }
      if(k < 0) continue;
      a[i][k] = 1;
    }
  }
  for(i=0;i<P;i++){
    for(j=0;j<P;j++)
      printf("%1d",a[i][j]);
    printf("\n");
  }
}
