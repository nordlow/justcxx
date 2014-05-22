#include "workpool.hpp"
#include "bitwise.h"
#include "bitget.h"
#include "timing.h"
#include "stdio_x.h"
#include "cpumult.h"

#ifdef HAVE_LIBAVFORMAT_LIBAVFORMAT_H
#  include "i.h"
#endif

#ifdef HAVE_FREEIMAGE_H
#  include "FreeImage.h"
#endif

/*!
 * General Work Load (Context).
 */
typedef struct WLoad {
  WLOAD_ID_t wId;               /**< Worker Load Identifier (Id). */
  WLOAD_t    wT;                /**< Worker Load \em Type. */

  void *(*wF)(void*);           /**< Worker \em Function. Calls argO = fun(argI) */

  union {
    void *        anonymous;    /**< Anonymous Load. */
    FLoadITex *   flit;         /**< File Load of Image Texture. */
    FSaveITex *   fsit;         /**< File Save of Image Texture. */
  } wD;                         /**< Worker \em Input Data. */

  intM_t prg;                   /**< Worker Load Progress. */

  uint            done_flag:1;	/**< Flags that work has been done. */
  pthread_mutex_t mutex;	/**< Mutex. */
} WLoad;

/*!
 * Put Work-Load \p wload to be \em empty (free).
 * \return 0 upon success, -1 otherwise.
 */
static int
wload_init_empty(WLoad * wload)
{
  int ret = 0;
  wload->wId = -1;
  wload->wT = WLOAD_undefined_;
  wload->wF = NULL;
  wload->wD.anonymous = NULL;
  intM_init(&wload->prg, 0.0f);
  wload->done_flag = 0;
  pthread_mutex_init(&wload->mutex, 0);
  return ret;
}

/*!
 * Mark Work-Load \p wload as \em empty (vacant).
 * \return 0 upon success, -1 otherwise.
 */
static int
wload_mark_empty(WLoad * wload)
{
  int ret = 0;

  switch (wload->wT) {
  case WLOAD_anonymous___:
    if (wload->wD.anonymous) { free(wload->wD.anonymous); wload->wD.anonymous = NULL; }
    break;
  case WLOAD_FLOADITEX:
    if (wload->wD.flit) { free(wload->wD.flit); wload->wD.flit = NULL; }
    break;
  case WLOAD_FSAVEITEX:
    if (wload->wD.fsit) { free(wload->wD.fsit); wload->wD.fsit = NULL; }
    break;
  case WLOAD_undefined_:
    /* nothing to \c free() */
    break;
  }

  wload->wId = -1;
  wload->wT = WLOAD_undefined_;
  wload->wF = NULL;
  intM_set(&wload->prg, 0.0f);
  wload->done_flag = 0;
  return ret;
}

/*!
 * Clear Work-Load \p wload.
 */
static void
wload_clear(WLoad * wload)
{
  wload_mark_empty(wload);
  pthread_mutex_destroy(&wload->mutex);
}

/*!
 * Try Putting Work-Load \p wload.
 * \return 0 upon success, -1 otherwise.
 */
static int
wload_tryput_FLoadITex(WLoad * wload,
                       WLOAD_ID_t wId, FILE * stream, DFMT_t dfmt)
{
  if (stream == NULL) { PERR("stream is NULL\n"); return -1; }
  int ret = -1;
  if (pthread_mutex_trylock(&wload->mutex) == 0) {
    if (wload->wT == WLOAD_undefined_) { /* if slot is free */
      if (wload->wD.anonymous != NULL) {
        PERR("wload->wD.anonymous != NULL"); return ret;
      }
      wload->wId = wId;
      wload->wT = WLOAD_FLOADITEX;
      wload->wF = NULL;
      wload->wD.flit = floaditex_new(stream, dfmt);
      intM_set(&wload->prg, 0.0f);
      wload->done_flag = 0;
      ret = 0;			/* indicate success */
    }
    pthread_mutex_unlock(&wload->mutex);
  }
  return ret;
}

/*!
 * Try Pulling Data from Work-Load \p wload.
 * \param[in] stream
 * \param[out] flit_ret
 * \return 1 upon success, 0 if \p wload no done yet, -1 otherwise.
 */
static int
wload_trypull_FLoadITex(WLoad * wload,
                        WLOAD_ID_t wId,
                        const FILE * stream,
                        FLoadITex * flit_ret)
{
  if (stream == NULL) { PERR("stream is NULL\n"); return -1; }
  int ret = -1;
  if (pthread_mutex_trylock(&wload->mutex) == 0) {
    if (wload->wId == wId   && /* if right id */
        wload->wT       == WLOAD_FLOADITEX && /* if right type */
        wload->wD.flit->fload.stream == stream) { /* same stream */
      if (wload->done_flag) {             /* if load done */
        *flit_ret = *wload->wD.flit;
        wload_mark_empty(wload);	/* mark slot as free */
        ret = 1;			/* indicate success */
      } else {
        ret = 0;
      }
    }
    pthread_mutex_unlock(&wload->mutex);
  }
  return ret;
}

/*!
 * Get Progress of Work-Load \p wload.
 * \return 1 upon success, -1 otherwise.
 */
static int
wload_rdProgress(WLoad * wload, int * prg_ret)
{
  int ret = -1;
  /* NOTE: No need to \c pthread_mutex_lock() here! */
  ret = intM_get(&wload->prg, prg_ret);
  return ret;
}

/*!
 * Try Loading Data from Work-Load \p wload.
 * \return 1 upon success, 0 if file format (\p dfmt) not supported,
 *         -1 otherwise.
 */
static int
wload_tryload(WLoad * wload)
{
  int ret = 0;
  if (pthread_mutex_trylock(&wload->mutex) == 0) {
    if (!wload->done_flag) {
      switch (wload->wT) {
      case WLOAD_anonymous___:
        PTODO("Handle case WLOAD_anoymous___\n");
        break;
      case WLOAD_FLOADITEX:
        if (wload->wD.flit->fload.stream && /* if slot has a stream */
            wload->wD.flit->itex.datT == 0) { /* that has not been loaded */
          if (freeimage_FILE_DFMT_loadFile_PXF(wload->wD.flit->fload.stream,
                                               wload->wD.flit->fload.dfmt,
                                               &wload->wD.flit->itex.w, &wload->wD.flit->itex.h,
                                               &wload->wD.flit->itex.pxf, &wload->wD.flit->itex.datT,
                                               &wload->prg) == 1) {
            ret = 1;            /* indicate loaded */
          } else {
            PTODO("Load file rawly into memory\n");
          }
          if (wload->wD.flit->itex.w == 0 ||
              wload->wD.flit->itex.h == 0 ||
              wload->wD.flit->itex.datT == 0) {
            PWARN("Load !failed\n");
            ret = -1;
          }
        }
        break;
      case WLOAD_FSAVEITEX:
        PTODO("Handle case WLOAD_FSAVEITEX\n");
        break;
      case WLOAD_undefined_:  /* do nothing. */ break;
      }
      wload->done_flag = 1;         /* flag as finished */
    }
    pthread_mutex_unlock(&wload->mutex);
  }
  return ret;
}

/* ---------------------------- Group Separator ---------------------------- */

/*! Maximum Number of Worker Threads (Workers) in Thread Pool. */
#define WPOOL_THREAD_MAXNUM (32)

/*! Worker Threads (Workers). */
pthread_t g_workers[WPOOL_THREAD_MAXNUM];

/*! Number of Worker Threads (Workers). */
static uint g_workersN = 1;

/* ---------------------------- Group Separator ---------------------------- */

#define WLOADS_MAXNUM (128) /**< Maximum number of pending loads reserved. */

/*!
 * Work Load Set.
 */
typedef struct WLoadS {
  WLoad           loads[WLOADS_MAXNUM];

  WLOAD_ID_t      next_wload_id; /**< Next Current Worker Load ID. */
  uint            defsN;        /**< Number of defined loads (pending or not) in set. */
  uint            pendsN;       /**< Number of pending loads. */

  uint            threads_exit_flag:1; /**< Flags that all threads should exit. */

  pthread_cond_t  pendsN_cond;	/**< Condition for pendsN. */
  pthread_mutex_t pendsN_cond_mutex; /**< Mutex for pendsN. */

  pthread_mutex_t set_mutex;	/**< Mutex for whole set. */
} WLoadS;

/* ---------------------------- Group Separator ---------------------------- */

/*!
 * Print Internals of \p wls.
 */
void
wloads_print(const WLoadS * wls)
{
  printf("WLoadS:\n");
  printf("- next_wload_id: %d\n", wls->next_wload_id);
  printf("- defsN: %d\n", wls->defsN);
  printf("- pendsN: %d\n", wls->pendsN);
}

/*!
 * Initialize \p wls.
 */
void
wloads_init_empty(WLoadS * wls)
{
  for (size_t i = 0; i < WLOADS_MAXNUM; i++) {
    WLoad * wload = &wls->loads[i];
    wload_init_empty(wload);
  }

  wls->next_wload_id     = 0; /* reset worker load id counter */
  wls->defsN             = 0; /* no loads yet */
  wls->pendsN            = 0; /* no pending loads yet */
  wls->threads_exit_flag = 0; /* no exiting yet */

  pthread_mutex_init(&wls->set_mutex, NULL);

  pthread_cond_init(&wls->pendsN_cond, NULL);
  pthread_mutex_init(&wls->pendsN_cond_mutex, NULL);

  PNOTE(" "); wloads_print(wls);
}

/*!
 * Clear \p wls and destroy their mutexes.
 */
void
wloads_clear(WLoadS * wls)
{
  if (wls->pendsN > 0) {
    PWARN("%d pending loads left.\n", wls->pendsN);
  }

  for (size_t i = 0; i < WLOADS_MAXNUM; i++) {
    WLoad * wload = &wls->loads[i];
    wload_clear(wload);
  }

  wls->next_wload_id     = 0; /* reset worker load id counter */
  wls->defsN             = 0; /* no loads anymore */
  wls->pendsN            = 0; /* no pending loads anymore */
  wls->threads_exit_flag = 0;

  pthread_mutex_destroy(&wls->set_mutex);

  pthread_cond_destroy(&wls->pendsN_cond);
  pthread_mutex_destroy(&wls->pendsN_cond_mutex);

  PNOTE(" "); wloads_print(wls);
}

/*!
 * Try Putting a load of the \p stream of format \p dfmt into \p wls.
 * \return >= 0 upon successful put, -1 otherwise.
 */
WLOAD_ID_t
wloads_tryput_load_FILE_DFMT(WLoadS * wls,
                             FILE * stream, DFMT_t dfmt)
{
  WLOAD_ID_t ret = -1;
  //size_t iF = WLOADS_MAXNUM;	/* index to free slot */
  if (pthread_mutex_lock(&wls->set_mutex) == 0) { /* NOTE: LOCK container */
    if (wls->defsN < WLOADS_MAXNUM) {	/* NOTE: Optimization */
      for (size_t i = 0; i < WLOADS_MAXNUM; i++) {
	WLoad * wload = &wls->loads[i];
	int lret = wload_tryput_FLoadITex(wload,
                                          wls->next_wload_id, stream, dfmt);
        if (lret >= 0) {
	  wls->next_wload_id += 1; /* increase worker load id */
	  wls->defsN         += 1; /* increase load counter */
	  wls->pendsN        += 1; /* increase pending counter */
	  /* signal at least 1 worker about new pending load */
	  if (pthread_cond_signal(&wls->pendsN_cond) != 0) {
	    lperror("Could not signal, pthread_cond_signal(): ");
	  }

	  //iF  = i;		/* log free slot index */
	  ret = wload->wId;     /* return load id */
	  break;		/* so were are done looping */
	}
      }
    } else {
      PNOTE("WLoadS has no free slots to put into.\n");
    }
    PNOTE(" "); wloads_print(wls);
    if (pthread_mutex_unlock(&wls->set_mutex) != 0) { /* NOTE: UNLOCK container */
      lperror("Could not unlock mutex, pthread_mutex_unlock(): ");
    }
  } else {
    lperror("Could not lock set_mutex, pthread_mutex_lock(): ");
  }
  return ret;
}

/*!
 * Try Pulling a load of the \p stream from \p wls.
 * \param[in] stream
 * \param[out] flit_ret
 * \return 1 upon success, 0 if load no done yet, -1 otherwise.
 */
int
wloads_trypull_FLoadITex(WLoadS * wls,
                         WLOAD_ID_t wId, const FILE * stream, FLoadITex * flit_ret)
{
  int ret = -1;
  if (pthread_mutex_lock(&wls->set_mutex) == 0) { /* NOTE: LOCK container */
    if ((wls->defsN - wls->pendsN) > 0) { /* NOTE: Optimization */
      for (size_t i = 0; i < WLOADS_MAXNUM; i++) {
	WLoad * wload = &wls->loads[i];
	int lret = wload_trypull_FLoadITex(wload, wId, stream, flit_ret);
	if (lret == 1) {
	  wls->defsN -= 1;	/* decrease load counter */
	  if (wls->defsN < wls->pendsN) {
	    PERR("This should not happend: defsN < pendsN\n");
	  }
	  ret = lret;		/* indicate success */
	  break;                /* so were are done looping */
	} else if (lret == 0) {
	  ret = lret;		/* indicate load not yet done */
	  break;                /* so were are done looping */
	} else {
          continue;             /* continue loop */
        }
      }
      PNOTE(" "); wloads_print(wls);
    } else {
      /* PNOTE("WLoadS has no finished slots to pull from.\n"); */
    }
    if (pthread_mutex_unlock(&wls->set_mutex) != 0) { /* NOTE: UNLOCK container */
      lperror("Could not unlock mutex, pthread_mutex_unlock(): ");
    }
  } else {
    lperror("Could not lock set_mutex, pthread_mutex_lock(): ");
  }
  return ret;
}

/*!
 * Get Progress of Work-Load having id \p wId.
 * \return 1 upon success, -1 otherwise.
 */
int
wloads_rdProgress(WLoadS * wls,
                  WLOAD_ID_t wId, int * prg_ret)
{
  int ret = -1;
  if (pthread_mutex_lock(&wls->set_mutex) == 0) { /* NOTE: LOCK container */
    for (size_t i = 0; i < WLOADS_MAXNUM; i++) {
      WLoad * wload = &wls->loads[i];
      if (wload->wId == wId) {
        ret = wload_rdProgress(wload, prg_ret);
      }
    }
    if (pthread_mutex_unlock(&wls->set_mutex) != 0) { /* NOTE: UNLOCK container */
      lperror("Could not unlock mutex, pthread_mutex_unlock(): ");
    }
  } else {
    lperror("Could not lock set_mutex, pthread_mutex_lock(): ");
  }
  return ret;
}

/*!
 * Try to Process all Pending Loads in \p wls.
 * \return number of loads processed, or -1 upon error.
 */
int
wloads_tryprocess_all(WLoadS * wls)
{
  int cnt = 0;
  if (wls->pendsN > 0) { /* NOTE: Optimization */
    for (size_t i = 0; i < WLOADS_MAXNUM; i++) {
      WLoad * wload = &wls->loads[i];
      pTimer tmr; ptimer_init(&tmr, CLOCK_THREAD_CPUTIME_ID); ptimer_tic(&tmr);
      if (wload_tryload(wload) == 1) {
	ptimer_toc(&tmr);
	/* WARNING: I AM UNSURE ABOUT THIS WAY OF LOCKING SOLUTION!!!. */
	if (pthread_mutex_lock(&wls->set_mutex) == 0) { /* NOTE: LOCK container */
	  wls->pendsN -= 1;	/* decrease pending counter */
	  PNOTE(" "); wloads_print(wls);
	  pthread_mutex_unlock(&wls->set_mutex); /* NOTE: UNLOCK container */
	} else {
	  lperror("Could not lock set_mutex, pthread_mutex_lock(): ");
	}
	PNOTE("Successful wload_tryload() took: "); ptimer_print_sec_usec9(tmr); endline();
	cnt += 1;			/* indicate load */
      }
    }
  } else {
    PNOTE("WLoadS has no pending slots to load.\n");
  }
  return cnt;
}

/*!
 * Process \p wls, until signalled to stop.
 * \return NULL.
 */
void*
wloads_process_loop(WLoadS * wls)
{
  void *ret = NULL;
  while (1) {			/* loop forever */
    if (wloads_tryprocess_all(wls) > 0) {
      /* continue loop */
    } else {			/* no pending loads yet */
      /* so sleep until boss adds and signals a load */
      pthread_mutex_lock(&wls->set_mutex); /* NOTE: LOCK container */
      while (wls->pendsN == 0 && /* while no pending loads and */
	     !wls->threads_exit_flag) { /* not flagged for exit */
	/* NOTE: atomically unlock and go to sleep */
	/* pthread_t self = pthread_self(); */
	PNOTE("Sleeping...\n");
	if (pthread_cond_wait(&wls->pendsN_cond,
			      &wls->set_mutex) != 0) {
	  lperror("pthread_cond_wait(): ");
	}
	PNOTE("Woke up...\n");
      }
      if (wls->threads_exit_flag) { /* if flagged for exit */
	PNOTE("Returning...\n");
	break;				/* break loop (which exits) */
      }
      PNOTE(" "); wloads_print(wls);
      if (pthread_mutex_unlock(&wls->set_mutex) != 0) { /* NOTE: UNLOCK container */
	lperror("Could not unlock mutex, pthread_mutex_unlock(): ");
      }
    }
  }
  return ret;
}

/* ---------------------------- Group Separator ---------------------------- */

static WLoadS g_wls;

int wpool_init_and_spawnThreads(uint worker_num)
{
  int ret = 1;
  wloads_init_empty(&g_wls);
#ifdef HAVE_FREEIMAGE_H
  FreeImage_Initialise(0);
#endif

  if (worker_num == 0) { worker_num = get_CPUmult(); }
  else if (worker_num > WPOOL_THREAD_MAXNUM) { worker_num = WPOOL_THREAD_MAXNUM; }
  g_workersN = worker_num;

  for (uint i = 0; i < g_workersN; i++) {
    if (pthread_create(&g_workers[i], NULL,
		       (void*)wloads_process_loop,
		       (void*)&g_wls) != 0) {
      lperror("Could not spawn worker, pthread_create(): ");
    }
    PNOTE("Spawned worker[%d]\n", i);
  }
  return ret;
}

int wpool_exit_joinThreads_and_clear(void)
{
  int ret = 1;

  /* \TODO: Functionize to wloads_flag_for_threads_exit(); */
  WLoadS * wls = &g_wls;
  if (pthread_mutex_lock(&wls->set_mutex) == 0) { /* NOTE: LOCK container */
    wls->threads_exit_flag = 1; /* flag threads to exit */
    /* signal at least all workers that they should exit */
    if (pthread_cond_broadcast(&wls->pendsN_cond) != 0) {
      lperror("Could not broadcast, pthread_cond_broadcast(): ");
    }
    pthread_mutex_unlock(&wls->set_mutex); /* NOTE: UNLOCK container */
  } else {
    lperror("Could not lock set_mutex, pthread_mutex_lock(): ");
  }

  sleep(3);
  return ret;

  /* \TODO: Functionize to wls/threadarray_join_threads(); */
  for (uint i = 0; i < g_workersN; i++) {
    PNOTE("Joining worker[%d]...\n", i);
    void * ret_value  = NULL;
    if (pthread_join(g_workers[i], &ret_value) == 0) {
      PNOTE("Joined worker[%d] that returned %p\n", i, ret_value);
    } else {
      lperror("Could not join worker, pthread_join(): ");
    }
  }

#ifdef HAVE_FREEIMAGE_H
  FreeImage_DeInitialise();
#endif

  wloads_clear(&g_wls);

  return ret;
}

/* ---------------------------- Group Separator ---------------------------- */

WLOAD_ID_t
wpool_tryput_load_FILE_DFMT(FILE * stream, DFMT_t dfmt)
{
  return wloads_tryput_load_FILE_DFMT(&g_wls, stream, dfmt);
}

WLOAD_ID_t
wpool_tryput_save_FILE_DFMT_buf(FILE * stream, DFMT_t dfmt,
                                const char *dbuf, size_t dlen)
{
  PTODO("\n");
  return 0;
}

/* ---------------------------- Group Separator ---------------------------- */

int
wpool_trypull_FLoadITex(WLOAD_ID_t wId, const FILE * stream, FLoadITex * flit_ret)
{
  return wloads_trypull_FLoadITex(&g_wls, wId, stream, flit_ret);
}

int
wpool_trypull_FSaveITex(WLOAD_ID_t wId, const FILE * stream, const FSaveITex * fsit)
{
  PTODO("Use freeimage to \\p fsit save \\p stream\n");
  return 0;
}

/* ---------------------------- Group Separator ---------------------------- */

int
wpool_rdProgress(WLOAD_ID_t wId, int * prg_ret)
{
  return wloads_rdProgress(&g_wls, wId, prg_ret);
}

/* ---------------------------- Group Separator ---------------------------- */

int
wpool_tryput_any(void *(*wF)(void*), void *warg)
{
  PTODO("\n");
  return 0;
}

int
wpool_trypull_any(void *warg)
{
  PTODO("\n");
  return 0;
}

/* ---------------------------- Group Separator ---------------------------- */

/*!
 * Convert DFMT \p dfmt to FreeImage Format (FIF) \c FIF and return it.
 */
int FIF_convertFrom_FKIND(DFMT_t dfmt)
{
  int fif = -1;
#ifdef HAVE_FREEIMAGE_H
  fif = FIF_UNKNOWN;
  switch (dfmt) {
  case DFMT_IMAGE_BMP: fif = FIF_BMP; break;
  case DFMT_IMAGE_ICO: fif = FIF_ICO; break;
  case DFMT_IMAGE_JPEG: fif = FIF_JPEG; break;
  case DFMT_IMAGE_JNG: fif = FIF_JNG; break;
  case DFMT_IMAGE_KOALA: fif = FIF_KOALA; break;
  case DFMT_IMAGE_LBM: fif = FIF_LBM; break;
  case DFMT_IMAGE_IFF: fif = FIF_IFF; break;
  case DFMT_IMAGE_MNG: fif = FIF_MNG; break;
  case DFMT_IMAGE_PBM: fif = FIF_PBM; break;
  case DFMT_IMAGE_PBMRAW: fif = FIF_PBMRAW; break;
  case DFMT_IMAGE_PCD: fif = FIF_PCD; break;
  case DFMT_IMAGE_PCX: fif = FIF_PCX; break;
  case DFMT_IMAGE_PGM: fif = FIF_PGM; break;
  case DFMT_IMAGE_PGMRAW: fif = FIF_PGMRAW; break;
  case DFMT_IMAGE_PNG: fif = FIF_PNG; break;
  case DFMT_IMAGE_PPM: fif = FIF_PPM; break;
  case DFMT_IMAGE_PPMRAW: fif = FIF_PPMRAW; break;
  case DFMT_IMAGE_RAS: fif = FIF_RAS; break;
  case DFMT_IMAGE_TARGA: fif = FIF_TARGA; break;
  case DFMT_IMAGE_TIFF: fif = FIF_TIFF; break;
  case DFMT_IMAGE_WBMP: fif = FIF_WBMP; break;
  case DFMT_IMAGE_PSD: fif = FIF_PSD; break;
  case DFMT_IMAGE_CUT: fif = FIF_CUT; break;
  case DFMT_IMAGE_XBM: fif = FIF_XBM; break;
  case DFMT_IMAGE_XPM: fif = FIF_XPM; break;
  case DFMT_IMAGE_DDS: fif = FIF_DDS; break;
  case DFMT_IMAGE_GIF: fif = FIF_GIF; break;
  case DFMT_IMAGE_HDR: fif = FIF_HDR; break;
  case DFMT_IMAGE_FAXG3: fif = FIF_FAXG3; break;
  case DFMT_IMAGE_SGI: fif = FIF_SGI; break;
  default: PWARN("Unsupported dfmt:%d\n", dfmt); break;
  }
#endif
  return fif;
}

/* ---------------------------- Group Separator ---------------------------- */

#ifdef HAVE_FREEIMAGE_H

static int
freeimage_FIBITMAP_loadPixels_1_PXF(FIBITMAP * dib, uint w, uint h,
                                    uint numC, uint pN,
                                    uint mR, uint mG, uint mB,
                                    PXF_t * pxf_ret, uchar ** datT_ret,
                                    intM_t * prg)
{
  int ret = 0;
  RGBQUAD *pal = FreeImage_GetPalette(dib); /* check palette */
  if (pal) {				  /* palette exists */
    if (0) {
      PNOTE("Palette (numC:%d)\n", numC);
      for (uint i = 0; i < numC; i++) {
	printf("  i:%d: [%d,%d,%d]\n", i,
	       pal[i].rgbRed, pal[i].rgbGreen, pal[i].rgbBlue);
      }
    }

    /* determine if all palette indexes are greyscale values */
    int all_grey = 1;
    for (uint i = 0; i < numC; i++) {
      if (pal[i].rgbRed != pal[i].rgbGreen ||
	  pal[i].rgbRed != pal[i].rgbBlue) {
	all_grey = 0;
      }
    }

    if (all_grey) { /* if all greyscale we use 8-bit greyscale texture */
      const uint dO = 1;	/* out pixel depth in bytes */
      uchar *datT = (uchar*)malloc(pN*dO);
      for (uint y = 0; y < h; y++) {
	const uchar *lineI = FreeImage_GetScanLine(dib, y);
	uchar *lineO = &datT[(h-1-y) * w*dO];
	for (uint x = 0; x < w; x++, lineO += dO) {
	  const uint idxBit = int8array_get_bit_MSBleft(lineI, x);
	  lineO[0] = pal[idxBit].rgbRed; /* red is also greyscale value */
	}
        if (prg) { intM_inc(prg); }
      }
      *pxf_ret = PXF_G8; *datT_ret = datT; ret = 1;
    } else {		/* otherwise we need 24-bit RGB texture */
      const uint dO = 3;	/* out pixel depth in bytes */
      uchar *datT = (uchar*)malloc(pN*dO);
      for (uint y = 0; y < h; y++) {
	const uchar *lineI = FreeImage_GetScanLine(dib, y);
	uchar *lineO = &datT[(h-1-y) * w*dO];
	for (uint x = 0; x < w; x++, lineO += dO) {
	  const uint idxB = int8array_get_bit_MSBleft(lineI, x);
	  lineO[0] = pal[idxB].rgbRed;
	  lineO[1] = pal[idxB].rgbGreen;
	  lineO[2] = pal[idxB].rgbBlue;
	}
        if (prg) { intM_inc(prg); }
      }
      *pxf_ret = PXF_RGB24; *datT_ret = datT; ret = 1;
    }
  }
  return ret;
}

static int
freeimage_FIBITMAP_loadPixels_4_PXF(FIBITMAP * dib, uint w, uint h,
                                    uint numC, uint pN,
                                    uint mR, uint mG, uint mB,
                                    PXF_t * pxf_ret, uchar ** datT_ret,
                                    intM_t * prg)
{
  int ret = 0;
  RGBQUAD *pal = FreeImage_GetPalette(dib); /* check palette */
  if (pal) {				  /* palette exists */
    if (1) {
      PNOTE("Palette (numC:%d)\n", numC);
      for (uint i = 0; i < numC; i++) {
	printf("  i:%d: [%d,%d,%d]\n", i,
	       pal[i].rgbRed, pal[i].rgbGreen, pal[i].rgbBlue);
      }
    }

    /* determine if all palette indexes are greyscale values */
    int all_grey = 1;
    for (uint i = 0; i < numC; i++) {
      if (pal[i].rgbRed != pal[i].rgbGreen ||
	  pal[i].rgbRed != pal[i].rgbBlue) {
	all_grey = 0;
      }
    }

    if (all_grey) { /* if all greyscale we use 8-bit greyscale texture */
      const uint dO = 1;	/* out pixel depth in bytes */
      uchar *datT = (uchar*)malloc(pN*dO);
      for (uint y = 0; y < h; y++) {
	const uchar *lineI = FreeImage_GetScanLine(dib, y);
	uchar *lineO = &datT[(h-1-y) * w*dO];
	for (uint x = 0; x < w; x++, lineO += dO) {
	  const uint qit = int8array_getQit_MSQleft(lineI, x);
	  lineO[0] = pal[qit].rgbRed; /* red is also greyscale value */
	}
        if (prg) { intM_inc(prg); }
      }
      *pxf_ret = PXF_G8; *datT_ret = datT; ret = 1;
    } else {		/* otherwise we need 24-bit RGB texture */
      const uint dO = 3;	/* out pixel depth in bytes */
      uchar *datT = (uchar*)malloc(pN*dO);
      for (uint y = 0; y < h; y++) {
	const uchar *lineI = FreeImage_GetScanLine(dib, y);
	uchar *lineO = &datT[(h-1-y) * w*dO];
	for (uint x = 0; x < w; x++, lineO += dO) {
	  const uint idxQ = int8array_getQit_MSQleft(lineI, x);
	  lineO[0] = pal[idxQ].rgbRed;
	  lineO[1] = pal[idxQ].rgbGreen;
	  lineO[2] = pal[idxQ].rgbBlue;
	}
        if (prg) { intM_inc(prg); }
      }
      *pxf_ret = PXF_RGB24; *datT_ret = datT; ret = 1;
    }
  }
  return ret;
}


static int
freeimage_FIBITMAP_loadPixels_8_PXF(FIBITMAP * dib, uint w, uint h,
                                    uint numC, uint pN,
                                    uint mR, uint mG, uint mB,
                                    PXF_t * pxf_ret, uchar ** datT_ret,
                                    intM_t * prg)
{
  int ret = 0;
  RGBQUAD *pal = FreeImage_GetPalette(dib); /* check palette */
  if (pal) {		/* if image has palette */
    const uint dI = 1;	/* in pixel depth in bytes */
    const uint dO = 3;	/* out pixel depth in bytes */
    if (0) {
      PNOTE("Palette (size:%d numC:%d)\n", 256, numC);
      for (uint i = 0; i < 256; i++) {
	printf("  i:%d: [%d,%d,%d]", i,
	       pal[i].rgbRed, pal[i].rgbGreen, pal[i].rgbBlue);
      }
    }
    uchar *datT = (uchar*)malloc(pN*dO);
    for (uint y = 0; y < h; y++) {
      const uchar *lineI = FreeImage_GetScanLine(dib, y);
      uchar *lineO = &datT[(h-1-y) * w*dO];
      for (uint x = 0; x < w; x++, lineI += dI, lineO += dO) {
	const uint8_t idxP = lineI[0]; /* palette index */
	lineO[0] = pal[idxP].rgbRed;
	lineO[1] = pal[idxP].rgbGreen;
	lineO[2] = pal[idxP].rgbBlue;
      }
      if (prg) { intM_inc(prg); }
    }
    *pxf_ret = PXF_RGB24; *datT_ret = datT; ret = 1;
  } else {		/* otherwise assume greyscale image */
    const uint dI = 1;	/* in pixel depth in bytes */
    const uint dO = 1;	/* out pixel depth in bytes */
    uchar *datT = (uchar*)malloc(pN*dO);
    for (uint y = 0; y < h; y++) {
      const uchar *lineI = FreeImage_GetScanLine(dib, y);
      uchar *lineO = &datT[(h-1-y) * w*dO];
      for (uint x = 0; x < w; x++, lineI += dI, lineO += 1) {
	const uint8_t grey = lineI[0]; /* greyscale value */
	lineO[0] = grey;
      }
      if (prg) { intM_inc(prg); }
    }
    *pxf_ret = PXF_G8; *datT_ret = datT; ret = 1;
  }
  return ret;
}

static int
freeimage_FIBITMAP_loadPixels_16_PXF(FIBITMAP * dib, uint w, uint h,
                                     uint numC, uint pN,
                                     uint mR, uint mG, uint mB,
                                     PXF_t * pxf_ret, uchar ** datT_ret,
                                     intM_t * prg)
{
  int ret = 0;
  if ((mR == FI16_565_RED_MASK) &&
      (mG == FI16_565_GREEN_MASK) &&
      (mB == FI16_565_BLUE_MASK)) {
    PTODO("RGB16 565 mode\n");
  } else {
    PTODO("RGB16 555 mode\n");
  }
  return ret;
}

static int
freeimage_FIBITMAP_loadPixels_24_PXF(FIBITMAP * dib, uint w, uint h,
                                     uint numC, uint pN,
                                     uint mR, uint mG, uint mB,
                                     PXF_t * pxf_ret, uchar ** datT_ret,
                                     intM_t * prg)
{
  int ret = 0;
  /* FI_RGBA_RED_MASK, FI_RGBA_GREEN_MASK, FI_RGBA_BLUE_MASK,  */
  const uint dI =	3;	/* in pixel depth in bytes */
  uchar *datT = (uchar*)malloc(pN*dI);
  for (uint y = 0; y < h; y++) {
    const uchar *lineI = FreeImage_GetScanLine(dib, y);
    uchar *lineO = &datT[(h-1-y) * w*dI];
    for (uint x = 0; x < w; x++, lineI += dI, lineO += dI) {
      lineO[0] = lineI[FI_RGBA_RED]; /* red */
      lineO[1] = lineI[FI_RGBA_GREEN]; /* green */
      lineO[2] = lineI[FI_RGBA_BLUE];  /* blue */
    }
    if (prg) { intM_inc(prg); }
  }
  *pxf_ret = PXF_RGB24; *datT_ret = datT; ret = 1;
  return ret;
}

static int
freeimage_FIBITMAP_loadPixels_32_PXF(FIBITMAP * dib, uint w, uint h,
                                     uint numC, uint pN,
                                     uint mR, uint mG, uint mB,
                                     PXF_t * pxf_ret, uchar ** datT_ret,
                                     intM_t * prg)
{
  int ret = 0;
  /* FI_RGBA_RED_MASK, FI_RGBA_GREEN_MASK, FI_RGBA_BLUE_MASK,  */
  const uint dI =	4;	/* in pixel depth in bytes */
  uchar *datT = (uchar*)malloc(pN*dI);
  for (uint y = 0; y < h; y++) {
    const uchar *lineI = FreeImage_GetScanLine(dib, y);
    uchar *lineO = &datT[(h-1-y) * w*dI];
    for (uint x = 0; x < w; x++, lineI += dI, lineO += dI) {
      lineO[0] = lineI[FI_RGBA_RED]; /* red */
      lineO[1] = lineI[FI_RGBA_GREEN]; /* green */
      lineO[2] = lineI[FI_RGBA_BLUE];  /* blue */
      lineO[3] = lineI[FI_RGBA_ALPHA];  /* alpha */
    }
    if (prg) { intM_inc(prg); }
  }
  *pxf_ret = PXF_RGBA32; *datT_ret = datT; ret = 1;
  return ret;
}

static int
freeimage_FIBITMAP_loadPixels_PXF(FIBITMAP * dib, uint w, uint h,
                                  uint bitsPP, uint numC, uint pN,
                                  uint mR, uint mG, uint mB,
                                  PXF_t * pxf_ret, uchar ** datT_ret,
                                  intM_t * prg)
{
  int ret = 0;
  const bool std_flag = 1;
  if (std_flag) {	/* standard bitmap depths */
    switch (bitsPP) {
    case 1:
      ret = freeimage_FIBITMAP_loadPixels_1_PXF(dib, w, h,
                                                numC, pN,
                                                mR, mG, mB,
                                                pxf_ret, datT_ret, prg);
      break;
    case 4:
      ret = freeimage_FIBITMAP_loadPixels_4_PXF(dib, w, h,
                                                numC, pN,
                                                mR, mG, mB,
                                                pxf_ret, datT_ret, prg);
      break;
    case 8:
      ret = freeimage_FIBITMAP_loadPixels_8_PXF(dib, w, h,
                                                numC, pN,
                                                mR, mG, mB,
                                                pxf_ret, datT_ret, prg);
      break;
    case 16:
      ret = freeimage_FIBITMAP_loadPixels_16_PXF(dib, w, h,
                                                 numC, pN,
                                                 mR, mG, mB,
                                                 pxf_ret, datT_ret, prg);
      break;
    case 24:
      ret = freeimage_FIBITMAP_loadPixels_24_PXF(dib, w, h,
                                                 numC, pN,
                                                 mR, mG, mB,
                                                 pxf_ret, datT_ret, prg);
      break;
    case 32:
      ret = freeimage_FIBITMAP_loadPixels_32_PXF(dib, w, h,
                                                 numC, pN,
                                                 mR, mG, mB,
                                                 pxf_ret, datT_ret, prg);
      break;
    case 48: PTODO("bitsPP:%d\n", bitsPP); break;
    case 64: PTODO("bitsPP:%d\n", bitsPP); break;
    case 96: PTODO("bitsPP:%d\n", bitsPP); break;
    case 128: PTODO("bitsPP:%d\n", bitsPP); break;
    default: PWARN("Cannot handle bitsPP %d\n", bitsPP); break;
    }
  } else {			/* non-standard bitmap depths */
    switch (bitsPP) {
    case 16: PTODO("bitsPP:%d\n", bitsPP); break;
    case 32: PTODO("bitsPP:%d\n", bitsPP); break;
    case 48: PTODO("bitsPP:%d\n", bitsPP); break;
    case 64: PTODO("bitsPP:%d\n", bitsPP); break;
    case 96: PTODO("bitsPP:%d\n", bitsPP); break;
    case 128: PTODO("bitsPP:%d\n", bitsPP); break;
    default: PWARN("Cannot handle bitsPP %d\n", bitsPP); break;
    }
  }
  return ret;
}
#endif

int
freeimage_FILE_DFMT_loadFile_PXF(FILE * stream, DFMT_t dfmt,
                                 uint * w_ret, uint * h_ret,
                                 PXF_t * pxf_ret, uchar **datT_ret,
                                 intM_t * prg)
{
  int ret = 0;
#ifdef HAVE_FREEIMAGE_H
  FreeImageIO io;
  io.read_proc = (FI_ReadProc)fread; /* pointer to function that calls fread */
  io.write_proc = NULL;    /* not needed for loading */
  io.seek_proc = (FI_SeekProc)fseek; /* pointer to function that calls fseek */
  io.tell_proc = (FI_TellProc)ftell; /* pointer to function that calls ftell */
  const int fif = FIF_convertFrom_FKIND(dfmt);
  if (fif != FIF_UNKNOWN) {
    FIBITMAP *dib = FreeImage_LoadFromHandle(fif, &io, (fi_handle)stream, 0);
    if (dib) {		/* bitmap successfully loaded! */
      const uint w = FreeImage_GetWidth(dib); /* Width */
      const uint h = FreeImage_GetHeight(dib); /* Height */
      *w_ret = w;				     /* write return width */
      *h_ret = h;				     /* write return height */
      const uint pN = w*h;		     /* Number of Pixels */

      const uint mR = FreeImage_GetRedMask(dib);	/* red mask */
      const uint mG = FreeImage_GetGreenMask(dib); /* green mask */
      const uint mB = FreeImage_GetBlueMask(dib);	 /* blue mask */

      const uint bitsPP = FreeImage_GetBPP(dib);  /* Bits Per Pixel */
      const uint lsz = FreeImage_GetLine(dib);	 /* Width of Bitmap in bytes */
      uint bytesPP = lsz/w; /* Bytes Per Pixel (3 at 24-bit, 4 at 32-bit) */
      bytesPP += 0;		/* NOTE: to prevent compilation warning */
      const uint numC = FreeImage_GetColorsUsed(dib); /* Number of Colors Used */

      ret = freeimage_FIBITMAP_loadPixels_PXF(dib, w, h,
                                              bitsPP, numC, pN,
                                              mR, mG, mB,
                                              pxf_ret, datT_ret, prg);
      FreeImage_Unload(dib);
    }
  } else {
    ret = -1;                   /* indicate error */
  }
#endif
  return ret;
}

void
freeimage_fd_DFMT_printMetaData(FILE * ostream, int fd, DFMT_t dfmt)
{
#ifdef HAVE_FREEIMAGE_H
  FreeImageIO io;
  io.read_proc = (FI_ReadProc)fread; /* pointer to function that calls fread */
  io.write_proc = NULL;    /* not needed for loading */
  io.seek_proc = (FI_SeekProc)fseek; /* pointer to function that calls fseek */
  io.tell_proc = (FI_TellProc)ftell; /* pointer to function that calls ftell */

  const int fif = FIF_convertFrom_FKIND(dfmt);
  if (fif != FIF_UNKNOWN) {
    FILE * stream = fdopen(fd, "r");
    if (stream) {		/* bitmap successfully loaded! */
      FIBITMAP *dib = FreeImage_LoadFromHandle(fif, &io, (fi_handle)stream, 0);

      unsigned count;
      if ((count = FreeImage_GetMetadataCount(FIMD_EXIF_GPS, dib))) {
        printf("count:%d\n", count);
      }

      if (dib) {		/* bitmap successfully loaded! */
        for (FREE_IMAGE_MDMODEL m = FIMD_COMMENTS;
             m < FIMD_CUSTOM; m++) { /* iterate over all metadata types */
          FITAG *tag = NULL;
          FIMETADATA *mdhandle = FreeImage_FindFirstMetadata(m, dib, &tag);
          if (mdhandle) {
            do {
              /* note that most tags do not have a description,
               * especially when the metadata specifications are not available */
              printf("key: %s\n", FreeImage_GetTagKey(tag));

              /* convert the tag value to a human readable string */
              const char *value = FreeImage_TagToString(m, tag, NULL);
              printf("value: %s\n", value);

              if (FreeImage_GetTagDescription(tag)) {
                printf("description: %s\n", FreeImage_GetTagDescription(tag));
              } else {
                printf("description: (none)\n");
              }
            } while (FreeImage_FindNextMetadata(mdhandle, &tag));
            FreeImage_FindCloseMetadata(mdhandle);
          }
        }
        FreeImage_Unload(dib);
      } else {
        PERR("FreeImage failed to open file\n");
      }
      fclose(stream);
    } else {
      PERR("Failed to open fd: %d\n", fd);
    }
  }
#endif
}

int FLoadITex_load(FLoadITex * flit)
{
  return freeimage_FILE_DFMT_loadFile_PXF(flit->fload.stream,
                                          flit->fload.dfmt,
                                          &flit->itex.w,
                                          &flit->itex.h,
                                          &flit->itex.pxf,
                                          &flit->itex.datT, NULL);
}

int FLoad_ITex_load(FLoad * fload_, ITex * itex)
{
  return freeimage_FILE_DFMT_loadFile_PXF(fload_->stream,
                                          fload_->dfmt,
                                          &itex->w,
                                          &itex->h,
                                          &itex->pxf,
                                          &itex->datT, NULL);
}
