#include "utils.h"
#include "alz3d.h"
#include "audioutils.h"
#include "fdutils.h"

#include "soundFXs.h"

#ifdef HAVE_CANBERRA_H
#include "canberra.h"
#endif

#include <errno.h>
#include <poll.h>
#include <pthread.h>

/* ========================================================================= */

/*!
 * Audiolizer main loop function to be spawned in a separate thread.
 * \param[in,out] arg not currently used
 */
static void * alz3d_writer(void * arg);

/* ---------------------------- Group Separator ---------------------------- */

static AudioCtx g_audio_ctx;

int g_alz3d_uopt = FALSE;
static int g_audiofd = -1;
static Alz3d g_alz;
static pthread_t g_alz_thread;
static int g_alz3d_swnum = 128;

/* ---------------------------- Group Separator ---------------------------- */

static SoundFX * g_sfxs = NULL;	/**< Pending Sound Effects Buffer. */
static size_t g_sfxsN = 0; /**< Number of Pending Sound Effects. */
static pthread_mutex_t g_soundFXs_mutex; /**< Assure Consistency for SoundFX-tranfers */

void soundFXs_init(void)
{
  if (g_alz3d_uopt) {
    /* audiolizer fd, mutex, thread */
    const char * devname = "/dev/audio";
    int schan = 2;
    int srate = 44100;
    int sprecision = 16;
    bool sign_flag = 1;
    bool little_endian_flag = FALSE;
    int frag = 0x0004000b;
    bool use_a3d_flag = TRUE;

    /* open audio device */
    g_audiofd = audioctx_open(&g_audio_ctx, devname, &schan, &srate,
			      &frag,
			      &sprecision, sign_flag, little_endian_flag);
    if (g_audiofd >= 0) {
      lprintf("Sound will be used (on /dev/audio)\n");
      unblockfd(g_audiofd);

      g_sfxs = NULL;
      g_sfxsN = 0;
      pthread_mutex_init(&g_soundFXs_mutex, 0);

      alz3d_init(&g_alz, schan, srate, sprecision, sign_flag,
		 little_endian_flag,
		 use_a3d_flag,
		 NULL);
      int rcode = pthread_create(&g_alz_thread, 0, alz3d_writer, 0);
      if (rcode < 0) { lperror("pthread_create"); }
    } else {
      lprintf("Sound will not be used\n");
      g_audiofd = -1;
    }
  }
}

void soundFXs_clear(void)
{
  if (g_alz3d_uopt) {
    /* initialize audiolizer fd, mutex, thread */
    alz3d_clear(&g_alz);
    if (g_audiofd >= 0) {
      close(g_audiofd);
      int sig = SIGKILL;		/* signal number */
      pthread_kill(g_alz_thread, sig);	/* kill audiolizer thread */
      pthread_mutex_destroy(&g_soundFXs_mutex);
    }
  }

  if (g_sfxs) { free(g_sfxs); g_sfxs = NULL; }
  g_sfxsN = 0;

  audioctx_close(&g_audio_ctx);
}

static void pthread_mutex_lock_warn(void)
{
  PWARN("pthread_mutex_lock() return != 0\n");
  switch (errno) {
  case EBUSY: lprintf("EBUSY\n"); break;
  case EINVAL: lprintf("EINVAL\n"); break;
  default: break;
  }
}

static void pthread_mutex_unlock_warn(void)
{
  switch (errno) {
  case EBUSY: lprintf("EBUSY\n"); break;
  case EINVAL: lprintf("EINVAL\n"); break;
  default: break;
  }
}

void
soundFXs_add(float fq, float tdur_sec, float azim, float amp)
{
  /*   lprintf("fq:%f\n", fq); */
  if (pthread_mutex_lock(&g_soundFXs_mutex) != 0) {
    pthread_mutex_lock_warn();
  } else {
    g_sfxs = (SoundFX*)realloc(g_sfxs, (g_sfxsN + 1) * sizeof(SoundFX));
    if (g_sfxs) {
      g_sfxs[g_sfxsN].fq = fq;
      g_sfxs[g_sfxsN].tdur_sec = tdur_sec;
      g_sfxs[g_sfxsN].azim = azim;
      g_sfxs[g_sfxsN].amp = amp;

      g_sfxsN++;
    } else {
      PWARN("Could not allocate memory for new sound effect\n\n");
    }

    int rval = pthread_mutex_unlock(&g_soundFXs_mutex);
    if (rval == 1) { pthread_mutex_unlock_warn(); }
  }
}

void soundFXs_add_xstr(const xStr * effect_name)
{
}

/*!
 * Transfer Sound-Effects to Audiolizer.
 *
 * \return 1 upon success, 0 otherwise.
 */
int
soundFXs_transferNow(Alz3d * alz)
{
  if (pthread_mutex_lock(&g_soundFXs_mutex) != 0) {
    pthread_mutex_lock_warn();
    return 0;
  } else {
    /* transfer sfxs to audiolizer */
    for (size_t i = 0; i < g_sfxsN; i++) {
      alz3d_addNow(alz,
		   g_sfxs[i].fq,
		   g_sfxs[i].tdur_sec,
		   g_sfxs[i].azim,
		   g_sfxs[i].amp,
		   ALZ3D_WAVE_SINE, ALZ3D_FADE_OUT);
    }

    /* clear sfx buffer */
    g_sfxsN = 0;
    g_sfxs = (SoundFX*)dynalloc(g_sfxs, (g_sfxsN) * sizeof(SoundFX));

    int rval = pthread_mutex_unlock(&g_soundFXs_mutex);
    if (rval == 1) { pthread_mutex_unlock_warn(); }

    return 1;
  }
}

/* ---------------------------- Group Separator ---------------------------- */

void * alz3d_writer(void * arg)
{
  struct pollfd ufds[1];
  const size_t nfds = 1;
  const int timeout = 100;		/* timeout in milliseconds */
  while (TRUE) {
    ufds[0].fd = g_audiofd;	/* one to poll */
    ufds[0].events = POLLOUT;	/* we check if we can write to it */
    int rval = poll(&ufds[0], nfds, timeout); /* check it */
    if (rval == 1) {
      if (ufds[0].revents & POLLOUT) {
	int ret_num;
	soundFXs_transferNow(&g_alz); /* if any transfer new sfx to audiolizer */
	/* write to audio device */
	ret_num = alz3d_write(&g_alz, g_audiofd, g_alz3d_swnum);
	/* lprintf("alz3d_write wrote %d of %d samples.", ret_num, g_alz3d_swnum); */
      }
    } else if (rval == 0) {
      /* do nothing */
    } else if (rval < 0) {
      lperror("poll");
    }
  }

  return 0;
}

/* ========================================================================= */
