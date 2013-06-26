#include <stdio.h>
#include <pthread.h>

#define NTHREADS 8
#define NLOOPS 200000000

static void *
worker (void *_cnt)
{
  int i;
  volatile int *cnt = _cnt;

  *cnt = 0;

  for (i = 0; i < NLOOPS; ++i)
    (*cnt)++;

  return 0;
}

static int cnt [NTHREADS][128];

int
main ()
{
  int i, sum;
  pthread_t t [NTHREADS];

  for (i = 0; i < NTHREADS; ++i)
    {
#ifdef BOUNCE
        pthread_create (&t [i], 0, worker, &cnt [0][i]);
#else
        pthread_create (&t [i], 0, worker, &cnt [i][0]);
#endif
    }

  sum = 0;
  for (i = 0; i < NTHREADS; ++i)
    {
      pthread_join (t [i], 0);
#ifdef BOUNCE
      sum += cnt [0][i];
#else
      sum += cnt [i][0];
#endif
    }

  printf ("%d\n", sum);
  return 0;
}
