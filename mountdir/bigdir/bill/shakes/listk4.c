
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>

#define N  13

int a[N][N];

int k4ct(int c);

int main(int argc, char *argv[])
{
   int i,j,k;

   for(i=0;i<N;i++)
      for(j=0;j<N;j++)
         scanf("%1d", &a[i][j]);
   k = k4ct(1);
   fprintf(stderr,"%d\n", k);
   exit(0);
}

int k4ct(int c)
{
   int i,j,k,l,t;
   t=0;
   for(i=0;i<N-3;++i)
      for(j=i+1;j<N-2;++j)
         if(a[i][j]==c)
            for(k=j+1;k<N-1;++k)
               if((a[i][k]==c)&&(a[j][k]==c))
                  for(l=k+1;l<N;++l)
                     if((a[i][l]==c)&&(a[j][l]==c)&&(a[k][l]==c)){
                       fprintf(stdout,"%5d%5d%5d%5d\n", i,j,k,l);
                       t++;
                     }
   return(t);
}
