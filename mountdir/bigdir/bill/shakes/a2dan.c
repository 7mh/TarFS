
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#define RND(X) (random()%(X))
#define DEG(X) b[X].deg

#define N    9

typedef
  unsigned char
uchar;

uchar a[N][N];

int main(int argc, char *argv[])
{
  int f,i,j,k,u,v;

  for(;;){
    for(i=0;i<N;i++){
      for(j=0;j<N;j++){
        if(scanf("%1d",&k) != 1) exit(0);
        a[i][j] = k;
      }
    }
    printf("E := ");
    f = 0;
    for(i=0;i<N-1;i++){
      for(j=i+1;j<N;j++){
        if(a[i][j]){
          printf("%c{%d,%d}", f ? ',' : '{', i+1, j+1);
          f = 1;
        }
      }
    }
    printf("}\n");
  }
}
