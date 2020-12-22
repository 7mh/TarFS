
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#define RND(X) (random()%(X))
#define DEG(X) b[X].deg

#define N   13

typedef
  unsigned char
uchar;

uchar a[N][N];

int main(int argc, char *argv[])
{
  int f,i,j,k,u,v;
  FILE *fd;

  fd = fopen("gbest","r");
  for(i=0;i<N;i++){
    for(j=0;j<N;j++){
      if(fscanf(fd,"%1d",&k) != 1) exit(0);
      a[i][j] = k;
    }
  }
}
