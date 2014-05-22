/*! \file workpool.hpp
 * \brief (Threaded) Pool of Workers Doing Image File Input/Output.
 * \author Copyright (C) 2012 Per Nordlöw (per.nordlow@gmail.com)
 * \date 2007-08-14 14:28
 *
 * Pool Spawns a set of threads that processes the Loading and Saving
 * Request of image files using the multithreading-safe FreeImage library.
 * For more details about FreeImage see
 *
 * \link http://freeimage.sourceforge.net/
 *
 * Boss typically:
 * - tries to place a load through \c wpool_tryput_load_FILE_DFMT(),
 * - waits a while for workers to do their job and then finally
 * - tries to fetch the result back through \c wpool_trypull_FLoadITex().
 *
 * IMPORTANT: My conclusion is that we need \em two categories of mutexes:
 * - \em Container Mutex: controls access to container structure, in this whose
 *   elements that are defined (the allocated size is fixed).
 *   - Putting Load (done by Boss(es):
 *     - \c wloadset_tryput()
 *       |-> \c wload_tryput()
 *   - Pulling Load (done by Boss(es)):
 *     - \c wloadset_trypull_FLoadITex()
 *       |-> \c wload_trypull()
 * - \em Element Mutex: controls changes "inside" this element.
 *   - Processing Load (done by Workers)
 *     - \c wloadset_process_forever()
 *       |-> \c wload_tryload()
 *
 * \todo Merge with \c pthreadpool.h
 */

#pragma once
#include "pnw_types.h"
#include "dfmt.h"
#include "pixels.h"
#include "atomic_ctypes.h"
#include <pthread.h>

/*!
 * Initialize Pool Handling Loading/Saving Images and Spawn @p
 * worker_num number of Worker Threads.
 * \param[in] worker_num Number of threads in pool. Use 0 to default to
 *                       number of CPUs online.
 */
int wpool_init_and_spawnThreads(uint worker_num);

/*!
 * Exit and Join Worker Threads and Clear Pool Handling Loading/Saving
 * Images.
 */
int wpool_exit_joinThreads_and_clear(void);

/* ---------------------------- Group Separator ---------------------------- */

/*!
 * WorkPool Load Identifier (ID).
 */
typedef int32_t WLOAD_ID_t;

/* ---------------------------- Group Separator ---------------------------- */

/*!
 * WorkPool Load (WorkLoad).
 */
typedef enum {
  WLOAD_anonymous___,           /**< Anonymous Load. */
  WLOAD_FLOADITEX,              /**< File Load of Image Texture. */
  WLOAD_FSAVEITEX,              /**< File Save of Image Texture. */
  WLOAD_undefined_,           /**< Undefined. */
} __attribute__ ((packed)) WLOAD_t;

/* ---------------------------- Group Separator ---------------------------- */

/*!
 * File Load Context.
 */
typedef struct FLoad {
  FILE * stream;		/**< File Stream to load from. */
  DFMT_t dfmt;			/**< File Format. */
} FLoad;

/* ---------------------------- Group Separator ---------------------------- */

/*!
 * Partial Read.
 */
typedef struct {
  int fd;                       /**< File to read from. */
  off_t off;                    /**< Offset to read from. */
  size_t count;                 /**< Number of bytes to read. */
  void *buf;                    /**< Buffer to read into. */
  ssize_t ret;                  /**< Return value from \c pread(). */
} PRead;

/*!
 * Partial Write.
 */
typedef struct {
  int fd;                       /**< File to read into. */
  off_t off;                    /**< Offset to write at. */
  size_t count;                 /**< Number of bytes to write. */
  void *buf;                    /**< Buffer to write from. */
  ssize_t ret;                  /**< Return value from \c pwrite(). */
} PWrite;

/* ---------------------------- Group Separator ---------------------------- */

/*!
 * File Save Context.
 */
typedef struct FSave {
  FILE * stream;		/**< File Stream to save to. */
  DFMT_t dfmt;			/**< File Format.*/
} FSave;

/* ---------------------------- Group Separator ---------------------------- */

/*!
 * Image Texture.
 */
typedef struct ITex {
  uint w;			/**< File/Texture Width. */
  uint h;			/**< File/Texture Height. */

  uchar *datT;			/**< Texture Pixel Data. */
  uint8_t datLOD;               /**< Number of Levels of Detail (LOD)
                                   starting at greatest/highest level. */

  PXF_t pxf;			/**< Texture Pixel Format. */
} ITex;

/* ---------------------------- Group Separator ---------------------------- */

typedef struct FLoadITex {
  FLoad fload;
  ITex itex;
} FLoadITex;

static inline void
floaditex_init(FLoadITex * flit,
               FILE * stream, DFMT_t dfmt)
{
  flit->fload.stream = stream;
  flit->fload.dfmt = dfmt;

  flit->itex.w = 0;
  flit->itex.h = 0;
  flit->itex.datT = NULL;
  flit->itex.datLOD = 0;
  flit->itex.pxf = PXF_UNKNOWN;
}

static inline
FLoadITex * floaditex_new(FILE * stream, DFMT_t dfmt)
{
  FLoadITex * flit = (FLoadITex*)malloc(sizeof(FLoadITex));
  floaditex_init(flit, stream, dfmt);
  return flit;
}

/* ---------------------------- Group Separator ---------------------------- */

typedef struct FSaveITex {
  FSave fsave;
  ITex itex;
} FSaveITex;

static inline void
fsaveitex_init(FSaveITex * fsit,
               FILE * stream, DFMT_t dfmt)
{
  fsit->fsave.stream = stream;
  fsit->fsave.dfmt = dfmt;

  fsit->itex.w = 0;
  fsit->itex.h = 0;
  fsit->itex.datT = NULL;
  fsit->itex.datLOD = 0;
  fsit->itex.pxf = PXF_UNKNOWN;
}

static inline
FSaveITex * fsaveitex_new(FILE * stream, DFMT_t dfmt)
{
  FSaveITex * fsit = (FSaveITex*)malloc(sizeof(FSaveITex));
  fsaveitex_init(fsit, stream, dfmt);
  return fsit;
}

/* ---------------------------- Group Separator ---------------------------- */

/*!
 * Try Putting a load on the function \p wfun together with the
 * argument data \p warg.
 * \return 0 upon successful put, -1 otherwise (workpool \em busy).
 */
int
wpool_tryput_any(void *(*wfun)(void*), void *warg);

/*!
 * Try Pulling a load operating on \p warg.
 * \return 0 upon successful pull, -1 otherwise (load not \em read).
 */
int
wpool_trypull_any(void *warg);

/* ---------------------------- Group Separator ---------------------------- */

/*!
 * Try Putting a \em load of the file \p stream of format \p dfmt.
 *
 * \return >= 0 upon successful put, -1 otherwise (workpool \em busy).
 */
WLOAD_ID_t
wpool_tryput_load_FILE_DFMT(FILE * stream, DFMT_t dfmt);

/*!
 * Try Putting a \em save of the data \p dbuf of length \p dlen to the
 * file \p stream of format \p dfmt.
 *
 * \return >= 0 upon successful put, -1 otherwise (workpool \em busy).
 */
WLOAD_ID_t
wpool_tryput_save_FILE_DFMT_buf(FILE * stream, DFMT_t dfmt,
                                const char *dbuf, size_t dlen);

/* ---------------------------- Group Separator ---------------------------- */

/*!
 * Try Pulling a \em Partial Read of the file \p fd.
 * \return 1 upon success, 0 if load no done yet, -1 otherwise.
 */
int
wpool_trypull_PRead(WLOAD_ID_t wId, PRead * pread_ret);

/*!
 * Try Pulling a \em Partial Write of the file \p fd.
 * \return 1 upon success, 0 if load no done yet, -1 otherwise.
 */
int
wpool_trypull_PWrite(WLOAD_ID_t wId, PWrite * pwrite_ret);

/* ---------------------------- Group Separator ---------------------------- */

/*!
 * Try Pulling a \em load of the file \p stream.
 *
 * \param[out] flit_ret File Image Image Texture.
 * \return 1 upon success, 0 if load no done yet, -1 otherwise.
 */
int
wpool_trypull_FLoadITex(WLOAD_ID_t wId, const FILE * stream, FLoadITex * flit_ret);

/*!
 * Try Pulling a \em save of the file \p stream.
 *
 * \return 1 upon success, 0 if save no done yet, -1 otherwise.
 */
int
wpool_trypull_FSaveITex(WLOAD_ID_t wId, const FILE * stream, const FSaveITex * fsit);

/* ---------------------------- Group Separator ---------------------------- */

/*!
 * Try Reading the Proress of the Work Load \p wId.
 *
 * \param[out] prg_ret Progress.
 * \return 1 upon success, -1 otherwise.
 */
int
wpool_rdProgress(WLOAD_ID_t wId, int * prg_ret);

/* ---------------------------- Group Separator ---------------------------- */

/*!
 * Load Image from file \p stream having file format \p dfmt.
 *
 * \param[out] w_ret Image Width upon return
 * \param[out] h_ret Image Height upon return
 * \param[out] pxf_ret Image Pixel Format upon return
 * \param[out] datT_ret Image Pixel Data upon return
 * \return 1 upon load, 0 upon no load, -1 upon error.
 */
int
freeimage_FILE_DFMT_loadFile_PXF(FILE * stream, DFMT_t dfmt,
                                 uint * w_ret, uint * h_ret,
                                 PXF_t * pxf_ret, uchar **datT_ret,
                                 intM_t * prg);


/*!
 * Print Image File Meta of the file \p fd of type \p dfmt to the
 * stream \p ostream:
 *
 * \param[out] prg_ret Progress.
 * \return 1 upon success, -1 otherwise.
 */
void
freeimage_fd_DFMT_printMetaData(FILE * ostream, int fd, DFMT_t dfmt);
