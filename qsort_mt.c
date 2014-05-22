#include <stdio.h>
#include <sys/cdefs.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <stdbool.h>
#include <pthread.h>
#include <sys/types.h>
#include <assert.h>
#include <errno.h>
#include <stdio.h>
#include "cpumult.h"

#if DEBUG_API
#define verify(x) do {				\
	int e;					\
	if ((e = x) != 0) {			\
		fprintf(stderr, "%s(%d) %s: %s\n",\
		    __FILE__, __LINE__,		\
		    #x, strerror(e)); 		\
		exit(1);			\
	}					\
} while(0)
#else /* !DEBUG_API */
#define verify(x) (x)
#endif

#if DEBUG_LOG
#define DLOG(...) fprintf(stderr, __VA_ARGS__)
#else
#define DLOG(...)
#endif

#ifdef I_AM_QSORT_R
typedef int cmp_t(void *, const void *, const void *);
#else
typedef int cmp_t(const void *, const void *);
#endif
static inline char *med3(char *, char *, char *, cmp_t *, void *);
static inline void swapfunc(char *, char *, int, int);

#define min(a, b)	(a) < (b) ? a : b

/*! Qsort routine from Bentley & McIlroy's "Engineering a Sort Function". */
#define swapcode(TYPE, parmi, parmj, n) { 		\
	long i = (n) / sizeof (TYPE); 			\
	TYPE *pi = (TYPE *) (parmi); 		\
	TYPE *pj = (TYPE *) (parmj); 		\
	do { 						\
		TYPE	t = *pi;		\
		*pi++ = *pj;				\
		*pj++ = t;				\
        } while (--i > 0);				\
}

static inline void
swapfunc(a, b, n, swaptype)
     char *a, *b;
     int n, swaptype;
{
  if (swaptype <= 1)
    swapcode(long, a, b, n)
      else
      swapcode(char, a, b, n)
}

#define swap(a, b)					\
	if (swaptype == 0) {				\
		long t = *(long *)(a);			\
		*(long *)(a) = *(long *)(b);		\
		*(long *)(b) = t;			\
	} else						\
		swapfunc(a, b, es, swaptype)

#define vecswap(a, b, n) 	if ((n) > 0) swapfunc(a, b, n, swaptype)

#ifdef I_AM_QSORT_R
#define	CMP(t, x, y) (cmp((t), (x), (y)))
#else
#define	CMP(t, x, y) (cmp((x), (y)))
#endif

static inline char *
med3(char *a, char *b, char *c, cmp_t * cmp, void *thunk
#ifndef I_AM_QSORT_R
//__unused
#endif
  )
{
  return CMP(thunk, a, b) < 0 ?
    (CMP(thunk, b, c) < 0 ? b : (CMP(thunk, a, c) < 0 ? c : a))
    : (CMP(thunk, b, c) > 0 ? b : (CMP(thunk, a, c) < 0 ? a : c));
}

/*! We use some elaborate condition variables and signalling
 * to ensure a bound of the number of active threads at
 * 2 * maxthreads and the size of the thread data structure
 * to maxthreads.
 */

/*! Condition of starting a new thread. */
enum THREAD_STATE
{
  THREAD_STATE_idle,			/* Idle, waiting for instructions. */
  THREAD_STATE_work,			/* Has work to do. */
  THREAD_STATE_term			/* Asked to terminate. */
};

/*! Quicksort Contex.
 *
 * Variant part passed to qsort invocations.
 */
struct qsort_ctx
{
  enum THREAD_STATE thread_state; /* For coordinating work. */
  struct common *common;	/* Common shared elements. */
  void *a;			/* Array base. */
  size_t n;			/* Number of elements. */
  pthread_t id;			/* Thread id. */
  pthread_mutex_t mtx_st;	/* For signalling state change. */
  pthread_cond_t cond_st;	/* For signalling state change. */
};

/*!
 * Invariant common part, shared across invocations.
 */
struct common
{
  int swaptype;			/* Code to use for swapping */
  size_t es;			/* Element size. */
  void *thunk;			/* Thunk for qsort_r */
  cmp_t *cmp;			/* Comparison function */
  int nthreads;			/* Total number of pool threads. */
  int idlethreads;		/* Number of idle threads in pool. */
  int forkelem;			/* Minimum number of elements for a new thread. */
  struct qsort_ctx *pool;	/* Fixed pool of threads. */
  pthread_mutex_t mtx_al;	/* For allocating threads in the pool. */
};

static void *qsort_thread(void *p);

void
qsort_mt(void *a, size_t n, size_t es, cmp_t * cmp,
	 unsigned int maxthreads, unsigned int forkelem)
{
  struct qsort_ctx *qs;
  struct common c;
  unsigned int i, islot;
  bool bailout = true;

  if (forkelem == 0) { forkelem = 4096; }
  if (n < forkelem) { goto f1; }

  errno = 0;

  if (maxthreads == 0) { maxthreads = get_CPUmult(); }

  /* Try to initialize the resources we need. */
  if (pthread_mutex_init(&c.mtx_al, NULL) != 0) { goto f1; }
  if ((c.pool = (struct qsort_ctx *) calloc(maxthreads, sizeof(struct qsort_ctx))) == NULL) { goto f2; }
  for (islot = 0; islot < maxthreads; islot++) {
    qs = &c.pool[islot];
    if (pthread_mutex_init(&qs->mtx_st, NULL) != 0) { goto f3; }
    if (pthread_cond_init(&qs->cond_st, NULL) != 0) {
      verify(pthread_mutex_destroy(&qs->mtx_st));
      goto f3;
    }
    qs->thread_state = THREAD_STATE_idle;
    qs->common = &c;
    if (pthread_create(&qs->id, NULL, qsort_thread, qs) != 0) {
      verify(pthread_mutex_destroy(&qs->mtx_st));
      verify(pthread_cond_destroy(&qs->cond_st));
      goto f3;
    }
  }

  /* All systems go. */
  bailout = false;

  /* Initialize common elements. */
  c.swaptype = ((char *) a - (char *) 0) % sizeof(long) ||
    es % sizeof(long) ? 2 : es == sizeof(long) ? 0 : 1;
  c.es = es;
  c.cmp = cmp;
  c.forkelem = forkelem;
  c.idlethreads = c.nthreads = maxthreads;

  /* Hand out the first work batch. */
  qs = &c.pool[0];
  verify(pthread_mutex_lock(&qs->mtx_st));
  qs->a = a;
  qs->n = n;
  qs->thread_state = THREAD_STATE_work;
  c.idlethreads--;
  verify(pthread_cond_signal(&qs->cond_st));
  verify(pthread_mutex_unlock(&qs->mtx_st));

  /*
   * Wait for all threads to finish, and
   * free acquired resources.
   */
 f3:
  for (i = 0; i < islot; i++) {
    qs = &c.pool[i];
    if (bailout) {
      verify(pthread_mutex_lock(&qs->mtx_st));
      qs->thread_state = THREAD_STATE_term;
      verify(pthread_cond_signal(&qs->cond_st));
      verify(pthread_mutex_unlock(&qs->mtx_st));
    }
    verify(pthread_join(qs->id, NULL));
    verify(pthread_mutex_destroy(&qs->mtx_st));
    verify(pthread_cond_destroy(&qs->cond_st));
  }
  free(c.pool);
 f2:
  verify(pthread_mutex_destroy(&c.mtx_al));
  if (bailout) {
    DLOG("Resource initialization failed; bailing out.\n");
    /* XXX should include a syslog call here */
    fprintf(stderr, "Resource initialization failed; bailing out.\n");
  f1:
    qsort(a, n, es, cmp);
  }
}

#define thunk NULL

/*!
 * Allocate an idle thread from the pool, lock its mutex, change its
 * state to work, decrease the number of idle threads, and return a
 * pointer to its data area.
 *
 * \return NULL, if no thread is available.
 */
static struct qsort_ctx *
allocate_thread(struct common *c)
{
  int i;

  verify(pthread_mutex_lock(&c->mtx_al));
  for (i = 0; i < c->nthreads; i++)
    if (c->pool[i].thread_state == THREAD_STATE_idle) {
      c->idlethreads--;
      c->pool[i].thread_state = THREAD_STATE_work;
      verify(pthread_mutex_lock(&c->pool[i].mtx_st));
      verify(pthread_mutex_unlock(&c->mtx_al));
      return (&c->pool[i]);
    }
  verify(pthread_mutex_unlock(&c->mtx_al));
  return (NULL);
}

/*!
 * Thread-callable quicksort.
 */
static void
qsort_algo(struct qsort_ctx *qs)
{
  char *pa, *pb, *pc, *pd, *pl, *pm, *pn;
  int d, r, swaptype, swap_cnt;
  void *a;			/* Array of elements. */
  size_t n, es;			/* Number of elements; size. */
  cmp_t *cmp;
  int nl, nr;
  struct common *c;
  struct qsort_ctx *qs2;

  /* Initialize qsort arguments. */
#if DEBUG_LOG
  pthread_t id = qs->id;
#endif
  c = qs->common;
  es = c->es;
  cmp = c->cmp;
  swaptype = c->swaptype;
  a = qs->a;
  n = qs->n;
top:
  DLOG("%10x n=%-10d Sort starting.\n", id, n);
#if DEBUG_SORT
  for (size_t i = 0; i < n; i++) { fprintf(stderr, "%d ", ((int *) a)[i]); }
  putc('\n', stderr);
#endif

  /* From here on qsort(3) business as usual. */
  swap_cnt = 0;
  if (n < 7) {
    for (pm = (char *) a + es; pm < (char *) a + n * es; pm += es) {
      for (pl = pm; pl > (char *) a && CMP(thunk, pl - es, pl) > 0; pl -= es) {
	swap(pl, pl - es);
      }
    }
    return;
  }
  pm = (char *) a + (n / 2) * es;
  if (n > 7) {
    pl = a;
    pn = (char *) a + (n - 1) * es;
    if (n > 40) {
      d = (n / 8) * es;
      pl = med3(pl, pl + d, pl + 2 * d, cmp, thunk);
      pm = med3(pm - d, pm, pm + d, cmp, thunk);
      pn = med3(pn - 2 * d, pn - d, pn, cmp, thunk);
    }
    pm = med3(pl, pm, pn, cmp, thunk);
  }
  swap(a, pm);
  pa = pb = (char *) a + es;

  pc = pd = (char *) a + (n - 1) * es;
  for (;;) {
    while (pb <= pc && (r = CMP(thunk, pb, a)) <= 0) {
      if (r == 0) {
	swap_cnt = 1;
	swap(pa, pb);
	pa += es;
      }
      pb += es;
    }
    while (pb <= pc && (r = CMP(thunk, pc, a)) >= 0) {
      if (r == 0) {
	swap_cnt = 1;
	swap(pc, pd);
	pd -= es;
      }
      pc -= es;
    }
    if (pb > pc) { break; }
    swap(pb, pc);
    swap_cnt = 1;
    pb += es;
    pc -= es;
  }
  if (swap_cnt == 0) {	       /* Switch to insertion sort */
    for (pm = (char *) a + es; pm < (char *) a + n * es; pm += es) {
      for (pl = pm; pl > (char *) a && CMP(thunk, pl - es, pl) > 0; pl -= es) {
	swap(pl, pl - es);
      }
    }
    return;
  }

  pn = (char *) a + n * es;
  r = min(pa - (char *) a, pb - pa);
  vecswap(a, pb - r, r);
  r = min(pd - pc, pn - pd - es);
  vecswap(pb, pn - r, r);

  nl = (pb - pa) / es;
  nr = (pd - pc) / es;
  DLOG("%10x n=%-10d Partitioning finished ln=%d rn=%d.\n", id, n, nl, nr);

  /* Now try to launch subthreads. */
  if (nl > c->forkelem &&
      nr > c->forkelem &&
      (qs2 = allocate_thread(c)) != NULL) {
    DLOG("%10x n=%-10d Left farmed out to %x.\n", id, n, qs2->id);
    qs2->a = a;
    qs2->n = nl;
    verify(pthread_cond_signal(&qs2->cond_st));
    verify(pthread_mutex_unlock(&qs2->mtx_st));
  } else if (nl > 0) {
    DLOG("%10x n=%-10d Left will be done in-house.\n", id, n);
    qs->a = a;
    qs->n = nl;
    qsort_algo(qs);
  }
  if (nr > 0) {
    DLOG("%10x n=%-10d Right will be done in-house.\n", id, n);
    a = pn - nr * es;
    n = nr;
    goto top;
  }
}

/*!
 * Thread-callable quicksort.
 */
static void *
qsort_thread(void *p)
{
  struct qsort_ctx *qs, *qs2;
  int i;
  qs = p;
#if DEBUG_LOG
  pthread_t id = qs->id;
#endif
  struct common *c = qs->common;
again:
  /* Wait for work to be allocated. */
  DLOG("%10x n=%-10d Thread waiting for work.\n", id, 0);
  verify(pthread_mutex_lock(&qs->mtx_st));
  while (qs->thread_state == THREAD_STATE_idle) {
    verify(pthread_cond_wait(&qs->cond_st, &qs->mtx_st));
  }
  verify(pthread_mutex_unlock(&qs->mtx_st));
  if (qs->thread_state == THREAD_STATE_term) {
    DLOG("%10x n=%-10d Thread signalled to exit.\n", id, 0);
    return 0;
  }
  assert(qs->thread_state == THREAD_STATE_work);

  qsort_algo(qs);

  verify(pthread_mutex_lock(&c->mtx_al));
  qs->thread_state = THREAD_STATE_idle;
  c->idlethreads++;
  DLOG("%10x n=%-10d Finished idlethreads=%d.\n", id, 0, c->idlethreads);
  if (c->idlethreads == c->nthreads) {
    DLOG("%10x n=%-10d All threads idle, signalling shutdown.\n", id, 0);
    for (i = 0; i < c->nthreads; i++) {
      qs2 = &c->pool[i];
      if (qs2 == qs) { continue; }
      verify(pthread_mutex_lock(&qs2->mtx_st));
      qs2->thread_state = THREAD_STATE_term;
      verify(pthread_cond_signal(&qs2->cond_st));
      verify(pthread_mutex_unlock(&qs2->mtx_st));
    }
    DLOG("%10x n=%-10d Shutdown signalling complete.\n", id, 0);
    verify(pthread_mutex_unlock(&c->mtx_al));
    return 0;
  }
  verify(pthread_mutex_unlock(&c->mtx_al));
  goto again;
}
