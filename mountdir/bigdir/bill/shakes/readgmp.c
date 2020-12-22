
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <ctype.h>
#include <gmp.h>

#define FNAME "g194.data"

#define N      194
#define K     4096

char *udstring = "7600970831036685591111201407664";

typedef struct vertex {
  mpq_t v[6];
} VERTEX;

VERTEX vlist[N];

mpq_t t0,t1,t2,t3,t4,t5,t6;
mpq_t ud, udsq;

int norm(mpq_t ans[2], VERTEX *p);
void diff(VERTEX *d, VERTEX *p, VERTEX *q);

int main(int argc, char *argv[])
{
  mpq_t ans[2];
  mpz_t x;
  int i,j,k,t;
  char b[K];
  char w[K];
  FILE *fd;
  char c;

  for(i=0;i<N;i++)
    for(j=0;j<6;j++)
      mpq_init(vlist[i].v[j]);
  mpq_init(t1);
  mpq_init(t2);
  mpq_init(t3);
  mpq_init(t4);
  mpq_init(t5);
  mpq_init(t6);
  mpq_init(ans[0]);
  mpq_init(ans[1]);
  mpq_init(ud);
  mpq_init(udsq);

  mpq_set_str(ud, udstring, 10);
  mpq_mul(udsq, ud, ud);

  fd = fopen(FNAME,"r");
  if(!fd){
    perror("opening FNAME");
    exit(0);
  }
  i = j = t = 0;
  while(fgets(b,K,fd)){
    j = 0;
    for(k=0;b[k];k++){
      if(!isspace(b[k])){
        w[t++] = b[k];
      } else {
        w[t] = 0;
        mpq_set_str(vlist[i].v[j], w, 10);
#ifdef DEBUG
        printf("%3d,%2d: %s\n", i,j,w);
#endif
        j++;
        t = 0;
      }
    }
    i++;
  }
  for(i=0;i<N;i++){
    norm(ans, vlist+i);
    if(mpq_equal(ans[0],udsq) && mpq_cmp_ui(ans[1],0,1) == 0)
      printf("%3d\n",i);
  }
#ifdef DEBUG
  for(i=0;i<N;i++){
    printf("%3d:\n",i);
    for(j=0;j<6;j++){
      mpq_out_str(stdout,10,vlist[i].v[j]);
      printf("\n");
    }
    norm(ans, vlist+i);
    mpq_out_str(stdout,10,ans[0]);
    printf("\n");
    mpq_out_str(stdout,10,ans[1]);
    printf("\n");
    getchar();
  }
#endif

}
int norm(mpq_t ans[2], VERTEX *p)
{
  mpq_mul(t0,p->v[0],p->v[0]);
  mpz_mul_ui(mpq_numref(t0), mpq_numref(t0), (unsigned long) 2);
  mpq_canonicalize(t0);
  mpq_mul(t1,p->v[1],p->v[1]);
  mpz_mul_ui(mpq_numref(t1), mpq_numref(t1), (unsigned long) 23);
  mpq_canonicalize(t1);
  mpq_add(t6,t0,t1);

  mpq_mul(t2,p->v[2],p->v[2]);
  mpq_add(t6,t6,t2);

  mpq_mul(t3,p->v[3],p->v[3]);
  mpz_mul_ui(mpq_numref(t3), mpq_numref(t3), (unsigned long) 46);
  mpq_canonicalize(t3);
  mpq_add(t6,t6,t3);

  mpq_mul(t4,p->v[4],p->v[4]);
  mpq_add(t6,t6,t4);

  mpq_mul(t5,p->v[5],p->v[5]);
  mpz_mul_ui(mpq_numref(t5), mpq_numref(t5), (unsigned long) 46);
  mpq_canonicalize(t5);
  mpq_add(ans[0],t6,t5);

  mpq_mul(t1,p->v[0],p->v[1]);
  mpq_mul(t2,p->v[2],p->v[3]);
  mpq_add(ans[1],t1,t2);
  mpq_mul(t3,p->v[4],p->v[5]);
  mpq_add(ans[1],ans[1],t3);
  mpz_mul_ui(mpq_numref(ans[1]), mpq_numref(ans[1]), (unsigned long) 2);
  mpq_canonicalize(ans[1]);
}

#ifdef XYZZY
def norm(d):
  global udsq

  x = [2*d[0]*d[0] + 23*d[1]*d[1], 0, 0, 2*d[0]*d[1]]
  y = [  d[2]*d[2] + 46*d[3]*d[3], 0, 0, 2*d[2]*d[3]]
  z = [  d[4]*d[4] + 46*d[5]*d[5], 0, 0, 2*d[4]*d[5]]
  e = [x[i]+y[i]+z[i] for i in range(4)]
  for i in range(1,4):
    if e[i] != 0:
      return False
  return e[0] == udsq

 x=a1*sqrt(2)+a2*sqrt(23)
 y=a3+a4*sqrt(2)*sqrt(23)
 z=a5+a6*sqrt(2)*sqrt(23)
#endif
void diff(VERTEX *d, VERTEX *p, VERTEX *q)
{
  int i;

  for(i=0;i<6;i++)
    mpq_sub(d->v[i],p->v[i],q->v[i]);
}
