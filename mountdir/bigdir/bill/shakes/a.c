
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>

typedef
    unsigned char
uchar;

#define RND(X)  (lrand48() % (X))

#define N          81

#define LM        100
#define NF          1

/*
 * T = # unused edges
 * S = # 4-isets
 * R = # number of 4-isets per edge
 */

#define R          12
#define S        1944
#define T         972

#define SAVEFILE "g81"

uchar a[N][N];
uchar used[T];
int hit[S];
int iset[S][4];
int nonedge[T][2];
int hitlist[T][R];
int deg[N];
int hits;
int nedges;

int k4ct(int c);
int ek4ct(int c, int i, int j);
void savegraph();
int make_nonedges();
int make_hitlists();
int getvalue(int k);
void shuffle(int sig[]);
void fiddle(int rmax);
void unfiddle(int n);

int whoami;
FILE *logfd;

int main(int argc, char *argv[])
{
    int sig[T];
    int i,j,k,m,t,u,v;
    int ptr;
    int iter,limp,lbest,best,sum;
    uchar flag;
    FILE *fd;
    char filename[64];

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
      setpriority(0, 0, 19);
    } else {
      logfd = stderr;
      srand48(getpid());
    }
    memset(a,0,N*N);
    fd = fopen("hamming43.txt","r");
    m = 0;
    while(fscanf(fd,"%d %d", &u, &v) == 2){
        u--; v--;
        a[u][v] = a[v][u] = 1;
        m++;
    }
    fclose(fd);

    k = make_nonedges();
    fprintf(logfd,"%d edges, %d non-edges\n", m, k);

    best = lbest = k4ct(0);
    fprintf(logfd,"%d independent 4-sets\n",best);

    make_hitlists();

    best = 0;
    hits = 0;
    nedges = 0;
    memset(hit,0,S*sizeof(int));
    memset(used,0,T);
    memset(deg,0,N*sizeof(int));
    for(;;){
        fiddle(R-4);
        if(hits > best){
            best = hits;
            fprintf(logfd,"%3d%6d%6d%6d\n", whoami, iter, nedges, hits);
            if(hits == S){
                sprintf(filename,"elist.%03d",whoami);
                fd = fopen(filename,"w");
                for(i=0;i<T;i++)
                    if(used[i])
                        fprintf(fd,"%2d%4d\n",nonedge[i][0],nonedge[i][1]);
                fclose(fd);
                exit(0);
            }
        }
        unfiddle(whoami);
    }
}
void fiddle(int rmax)
{
    int iter,ptr,flag;
    int i,j,k,n;
    int sig[T];

    shuffle(sig);
    ptr = 0;
    for(;;){
        flag = 0;
        while(ptr < T){
            k = sig[ptr++];
            if(!used[k] && getvalue(k) >= rmax){
                flag = 1;
                break;
            }
        }
        if(!flag)
            break;
        used[k] = 1;
        for(i=0;i<R;i++){
            j = hitlist[k][i];
            if(!hit[j])
                hits++;
            hit[j]++;
        }
        deg[nonedge[k][0]]++;
        deg[nonedge[k][1]]++;
        nedges++;
    }
}
void unfiddle(int n)
{
    int i,j,k;
    int ptr;
    int sig[T];

    shuffle(sig);
    for(ptr=0;ptr<T;ptr++){
        if(n <= 0)
            break;
        k = sig[ptr];
        if(!used[k])
            continue;
        used[k] = 0;
        for(i=0;i<R;i++){
            j = hitlist[k][i];
            hit[j]--;
            if(!hit[j])
                hits--;
        }
        deg[nonedge[k][0]]--;
        deg[nonedge[k][1]]--;
        nedges--;
        n--;
    }
}
int getvalue(int k)
{
    int i,j;

    for(i=j=0;i<R;i++)
        if(!hit[hitlist[k][i]])
            j++;
    return j;
}
int contains(int is[], int u, int v)
{
    int i,k;

    k = 0;
    for(i=0;i<4;i++){
        if(is[i] == u) k++;
        if(is[i] == v) k++;
    }
    return k == 2;
}
int make_hitlists()
{
    int i,j,k,u,v;
    int hc[T];

    memset(hc,0,T*sizeof(int));
    for(i=0;i<T;i++){
        u = nonedge[i][0];
        v = nonedge[i][1];
        for(j=0;j<S;j++){
            if(contains(iset[j],u,v)){
                hitlist[i][hc[i]++] = j;
            }
        }
    }
}
int make_nonedges()
{
    int i,j,k;

    k = 0;
    for(i=0;i<N-1;i++)
        for(j=i+1;j<N;j++)
            if(!a[i][j]){
                nonedge[k][0] = i;
                nonedge[k][1] = j;
                k++;
            }
    if(k != T){
        fprintf(logfd,"make_nonedges: %d != %d\n", k, T);
        exit(0);
    }
    return k;
}
void shuffle(int sig[])
{
    int i,j,k;

    for(i=0;i<T;i++)
        sig[i] = i;
    for(i=0;i<T;i++){
        j = RND(T);
        k = RND(T);
        if(j == k) continue;
        sig[j] ^= sig[k];
        sig[k] ^= sig[j];
        sig[j] ^= sig[k];
    }
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
                                iset[t][0] = i;
                                iset[t][1] = j;
                                iset[t][2] = k;
                                iset[t][3] = l;
                                t++;
                            }
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

