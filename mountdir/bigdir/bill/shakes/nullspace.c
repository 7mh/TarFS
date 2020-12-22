
/*
   incomplete
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <signal.h>
#include <time.h>
#include <sys/time.h>
#include <sys/resource.h>

#define NROWS        13
#define NCOLS        40

#define NF            1
#define LM          100

#define MAXITER 1000000
#define NVALS         2

#define RND(X)     (lrand48() % (X))
#define RNVAL      (1 + RND(NVALS))

typedef
unsigned char
uchar;

int a[NROWS][NCOLS];

void randvector(int u[]);
void image(int v[], int u[]);
void printvector(int v[], int n);
void zap(int ivec[]);
void savevector(FILE *fd, int ivec[], int n);
int fiddleone(int ivec[], int x);
int fiddle(int ivec[]);
int pick(int ct[]);
int score(int vec[]);

int whoami;
FILE *logfd;

int main(int argc, char *argv[])
{
    int iter,limp,sum,lbest,best;
    int i,j,k,n,u,v,w;
    int ivec[NCOLS];
    int ovec[NROWS];
    char filename[64];
    char fname[256];
    FILE *fd;

    whoami = atoi(argv[1]);
    if(whoami){
        srand48((whoami << 13)  ^ (getpid() << 7) ^ time(0));
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
        srand48(time(0));
    }
    fd = fopen("mat","r");
    for(i=0;i<NROWS;i++)
        for(j=0;j<NCOLS;j++){
            if(fscanf(fd,"%d", &k) != 1) exit(0);
            a[i][j] = k;
        }
    fclose(fd);

    sprintf(fname,"soln.%03d", whoami);
    fd = fopen(fname,"w");
    k = 0;
    for(k=0;;k++){
        while(!fiddle(ivec))
            ;
        fprintf(logfd,"%3d %6d\n", whoami, k);
        savevector(fd, ivec, NCOLS);
        if(k == 100000) break;
    }
    fclose(fd);
}
int fiddle(int ivec[])
{
    int iter,limp,sum,lbest,best;
    int i,j,k,n,u,v,w;
    int ovec[NROWS];

    randvector(ivec);
    image(ovec, ivec);
    best = lbest = score(ovec);
    fprintf(logfd,"%3d starts at %6d\n", whoami, best);
    limp = 0;
    for(iter=1;iter<=MAXITER;iter++){
        sum = fiddleone(ivec,RND(NCOLS));
        if(sum < lbest){
            lbest = sum;
            limp = 0;
            if(sum < best){
                best = sum;
                image(ovec, ivec);
                if(best == 0){
                    return 1;
                }
            }
        } else {
            limp++;
            if(limp == LM){
                zap(ivec);
                image(ovec,ivec);
                lbest = score(ovec);
                limp = 0;
            }
        }
    }
    return 0;
}
void savevector(FILE *fd, int ivec[], int n)
{
    int i;

    for(i=0;i<NCOLS;i++)
        fprintf(fd,"%2d",ivec[i]);
    fprintf(fd,"\n");
}
int score(int vec[])
{
    int i,j;

    for(i=j=0;i<NROWS;i++)
        j += abs(vec[i]); 
    return j;
}
void zap(int ivec[])
{
    int i, nf;

    nf = NF;
    for(i=0;i<nf;i++)
        ivec[RND(NCOLS)] = RNVAL;
}
int fiddleone(int ivec[], int x)
{
    int ovec[NROWS];
    int tally[NVALS+1];
    int i,j,k;

    for(i=1;i<=NVALS;i++){
        ivec[x] = i;
        image(ovec, ivec);
        tally[i] = score(ovec);
    }
    k = pick(tally);
    ivec[x] = k;
    return tally[k];
}
int pick(int ct[])
{
    int i,bc[4],m,n,d;

    m = 0x7fffffff;
    n = 0;
    for(i=1;i<=NVALS;i++){
        if(ct[i] < m){
            m = ct[i];
            n = 1;
            bc[0] = i;
        } else if(ct[i] == m){
            bc[n++] = i;
        }
    }
    if(n==1)
        return(bc[0]);
    else
        return(bc[RND(n)]);
}  
void randvector(int v[])
{
    int i;

    for(i=0;i<NCOLS;i++)
        v[i] = RNVAL;
}
void image(int ovec[], int ivec[])
{
    int i,j,k;

    for(i=0;i<NROWS;i++){
        k = 0;
        for(j=0;j<NCOLS;j++)
            k += a[i][j] * ivec[j];
        ovec[i] = k;
    }
}
void printvector(int v[], int n)
{
    int i,j,k;

    for(i=0;i<n;i++)
        fprintf(logfd, "%4d",v[i]);
    fprintf(logfd, "\n");
}

