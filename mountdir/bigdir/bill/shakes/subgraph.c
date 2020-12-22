
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define N            286
#define S             21
#define T             40

typedef
  unsigned char
uchar;

uchar *a[N];

int whoami;
FILE *logfd;

void dfs(int dep, int start, int nedges, int li[]);

int main(int argc, char *argv[])
{
  int i,j,k,u,v;
  int li[S];
  char filename[64];
  FILE *fd;

  whoami = atoi(argv[1]);
  if(whoami){
    srand48((whoami << 3)  ^ (getpid() << 7) ^ time(0));
    fclose(stdin);
    fclose(stdout);
    fclose(stderr);
    if(fork())
      exit(0);
    sprintf(filename,"log.%03d",whoami);
    logfd = fopen(filename,"w");
    setbuf(logfd,0);
    setpriority(0, 0, 20);
  } else {
    logfd = stderr;
    srand48(getpid());
  }

  fd = fopen("elist","r");
  if(!fd)
    exit(0);
  for(i=0;i<N;i++){
    a[i] = malloc(N);
    memset(a[i],0,N);
  }
  while(fscanf(fd,"%d %d", &u, &v) == 2){
    a[u][v] = a[v][u] = 1;
  }
  fclose(fd);
  fprintf(logfd,"done reading\n");
  li[0] = 0;
  dfs(1,1,0,li);
}
void dfs(int dep, int start, int nedges, int li[])
{
  int i,j,k;

  if(dep == S){
    if(nedges == T){
      for(i=0;i<S;i++){
        for(j=0;j<S;j++)
          fprintf(stdout,"%1d",a[li[i]][li[j]]);
        fprintf(stdout,"\n");
      }
    }
    return;
  }
  for(i=start+1;i<N;i++){
    k = 0;
    for(j=0;j<dep;j++)
      if(a[i][li[j]])
        k++;
    li[dep] = i;
    if(k <= T)
      dfs(dep+1,i+1,nedges+k,li);
  }
}
