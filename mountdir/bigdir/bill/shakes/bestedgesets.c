
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>

typedef
    unsigned char
uchar;

#define RND(X)  (lrand48() % (X))

#define N      81
#define NES    40
#define USE    28

#define LM    100
#define NF      3

#include "eli.h"

#define SAVEFILE  "g"

void savegraph();
void pickset();
int k4ct(int c);
int ek4ct(int c, int i, int j);
void remove_set(int k);
void insert_set(int k);
void zap(int n);

uchar used[NES];
uchar a[N][N];

int whoami;
FILE *logfd;

int main(int argc, char *argv[])
{
    int i,j,k,t;
    int k1,k2;
    int iter,limp,lbest,best,sum;
    char filename[64];

    whoami = atoi(argv[1]);
    if(whoami){
        srand48((whoami << 3)    ^ (getpid() << 7) ^ time(0));
        fclose(stdin);
        fclose(stdout);
        fclose(stderr);
        if(fork())
            exit(0);
        sprintf(filename,"log.%03d",whoami);
        logfd = fopen(filename,"w");
        setbuf(logfd,0);
        setpriority(0, 0, 19);
    } else {
        logfd = stderr;
        srand48(getpid());
    }
    srand48(getpid());
    for(i=0;i<NES;i++)
        for(j=0;j<N;j++)
            for(k=0;k<2;k++)
                eli[i][j][k] -= 1;
    pickset();
    best = lbest = k4ct(1);
    fprintf(logfd,"%d\n",best);
    limp = 0;
    for(iter=1;;iter++){
        do k1 = RND(NES); while(!used[k1]);
        do k2 = RND(NES); while(used[k2]);
        remove_set(k1);
        insert_set(k2);
        sum = k4ct(0);
        if(sum < lbest){
            lbest = sum;
            limp = 0;
            if(sum < best){
                best = sum;
                for(i=0;i<N;i++)
                    if(used[i])
                        fprintf(logfd,"%4d",i);
                fprintf(logfd,"\n");
                fprintf(logfd,"%3d%9d%9d\n",whoami,iter,best);
                if(best == 0){
                    savegraph();
                    exit(0);
                }
            }
        } else {
            insert_set(k1);
            remove_set(k2);
            limp++;
            if(limp == LM){
                zap(NF);
                lbest = k4ct(0);
                limp = 0;
            }
        }
    }
}
void zap(int n)
{
    int i,k1,k2;

    for(i=0;i<n;i++){
        do k1 = RND(NES); while(!used[k1]);
        do k2 = RND(NES); while(used[k2]);
        remove_set(k1);
        insert_set(k2);
    }
}
void remove_set(int k)
{
    int i,u,v;

    if(!used[k]){
        fprintf(logfd,"trying to remove_set list %d?\n", k);
        exit(0);
    }
    used[k] = 0;
    for(i=0;i<N;i++){
        u = eli[k][i][0];
        v = eli[k][i][1];
        a[u][v] = a[v][u] = 0;
    }
}
void insert_set(int k)
{
    int i,u,v;

    if(used[k]){
        fprintf(logfd,"trying to insert_set list %d?\n", k);
        exit(0);
    }
    used[k] = 1;
    for(i=0;i<N;i++){
        u = eli[k][i][0];
        v = eli[k][i][1];
        a[u][v] = a[v][u] = 1;
    }
}
void pickset()
{
    int i,j,k,u,v;

    memset(a,0,N*N);
    memset(used,1,NES);
    for(i=0;i<(NES-USE);i++){
        do j = RND(NES); while(!used[j]);
        used[j] = 0;
    }
    for(i=0;i<NES;i++){
        if(!used[i])
            continue;
        for(j=0;j<N;j++){
            u = eli[i][j][0];
            v = eli[i][j][1];
            a[u][v] = a[v][u] = 1;
        }
    }
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
                     if((a[i][l]==c)&&(a[j][l]==c)&&(a[k][l]==c))
                        t++;
   return(t);
}
int ek4ct(int c, int i, int j)
{
   int l,o;
   int k,tally;
   int list[N];

   o=0;
   for(k=0;k<N;++k)
      if((k!=i)&&(k!=j))
         if((c==a[i][k])&&(c==a[j][k]))
            list[o++]=k;
   tally=0;
   for(k=0;k<o-1;++k)
      for(l=k+1;l<o;++l)
         if(a[list[k]][list[l]] == c)
            tally++;
   return(tally);
}
void savegraph()
{
  int i,j;
  FILE *fd;

  fd = fopen(SAVEFILE, "w");
  for(i=0;i<N;i++){
    for(j=0;j<N;j++)
      fprintf(fd,"%1d",a[i][j]);
    fprintf(fd,"\n");
  }
  fclose(fd);
}

