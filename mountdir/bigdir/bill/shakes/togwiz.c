
#define P 1000

#define RND(X) (random() % (X))

#include <stdio.h>

int a[P][P];
int p;

main(int argc, char *argv[])
{
  FILE *fd;
  int af[P],nf[P];
  int i,j,k,v,bad;

  srandom(getpid());

  if(argc != 3){
    fprintf(stderr,"usage: %s p graph\n",argv[0]);
    exit(0);
  }
  p = atoi(argv[1]);
  fd = fopen(argv[2],"r");
  if(fd==NULL){
    fprintf(stderr,"usage: %s p graph\n",argv[0]);
    exit(0);
  }
  for(i=0;i<p;i++)
    for(j=0;j<p;j++)
      fscanf(fd,"%1d",&a[i][j]);
  fclose(fd);
  for(i=0;i<p;i++){
    printf("%d,%d",RND(600),RND(600));
    for(j=0;j<p;j++){
      if(i == j)
        continue;
      if(a[i][j])
        printf(",%d",j+1);
    }
    printf("\n");
  }
}
count_common(int u, int v)
{
  int i,j;

  for(i=j=0;i<p;i++)
    if((a[u][i]==1) && (a[v][i]==1))
      j++;
  return j;
}
printlist(int li[])
{
  int i,j;

  j=p-1;
  while((j>1)&&(li[j-1]==0))
    --j;
  for(i=0;i<j;i++)
    printf("%8d",li[i]);
  printf("\n");
}
