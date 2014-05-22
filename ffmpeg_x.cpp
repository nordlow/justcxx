#include "ffmpeg_x.hpp"
#include <iostream>
#include <cmath>
#include <climits>
#include <cstring>
#include <cstdlib>
#include <cerrno>
#include <unistd.h>
#include <pthread.h>

#if defined(HAVE_SDL_SDL_H)
#include <SDL/SDL.h>
#endif

void avformat_show(const AVFormatContext *ctx)
{
    using std::cout;
    using std::endl;

    if (ctx->filename) {
        cout << "Filename: " << ctx->filename << endl;
    }
    if (ctx->nb_streams) {
        cout << " Stream Count:" << ctx->nb_streams << endl;
    }

    if (ctx->start_time_realtime) {
        cout << " Start RealTime (since 00:00 1st January 1970):" << ctx->start_time_realtime << "[us]" << endl;
    }
    if (ctx->start_time) {// Decoding: position of the first frame of the component, in AV_TIME_BASE fractional seconds.
        cout << " StartTime:"
             << static_cast<double>(ctx->start_time) / AV_TIME_BASE
             << "[s]" << endl;
    }
    if (ctx->duration) { // Decoding: duration of the stream, in AV_TIME_BASE fractional seconds.
        cout << " Duration:"
             << static_cast<double>(ctx->duration) / AV_TIME_BASE
            // << '.'
            // << setw(pnw::binlog())
            // << ctx->duration % AV_TIME_BASE << '.'
             << "[s]" << endl;
    }
    if (ctx->bit_rate) {
        cout << " Bitrate:" << ctx->bit_rate << endl;
    }
    if (ctx->audio_codec_id != CODEC_ID_NONE ) {
        cout << " Audio Codec:" << ctx->audio_codec_id << endl;
    }
    if (ctx->video_codec_id != CODEC_ID_NONE ) {
        cout << " Video Codec:" << ctx->video_codec_id << endl;
    }
    if (ctx->subtitle_codec_id != CODEC_ID_NONE ) {
        cout << " Subtitle Codec:" << ctx->subtitle_codec_id << endl;
    }
    if (ctx->iformat) {
        if (ctx->iformat->name) {
            cout << " Output Format Name:" << ctx->iformat->name << endl;
        }
        if (ctx->iformat->long_name) {
            cout << " Output Format Name:" << ctx->iformat->long_name << endl;
        }
    }
    if (ctx->oformat) {
        if (ctx->oformat->name) {
            cout << " Output Format Name:" << ctx->oformat->name << endl;
        }
        if (ctx->oformat->long_name) {
            cout << " Output Format Name:" << ctx->oformat->long_name << endl;
        }
        if (ctx->oformat->mime_type) {
            cout << " Output Format Name:" << ctx->oformat->mime_type << endl;
        }
        if (ctx->oformat->audio_codec != CODEC_ID_NONE) {
            cout << " Output Format Audio Codec:" << ctx->oformat->audio_codec << endl;
        }
        if (ctx->oformat->video_codec != CODEC_ID_NONE) {
            cout << " Output Format Video Codec:" << ctx->oformat->video_codec << endl;
        }
    }
}

/* ========================================================================= */

#if 0

typedef struct {
    const char *name;
    int flags;
#  define HAS_ARG    (0x0001)
#  define OPT_BOOL   (0x0002)
#  define OPT_EXPERT (0x0004)
#  define OPT_STRING (0x0008)
#  define OPT_VIDEO  (0x0010)
#  define OPT_AUDIO  (0x0020)
#  define OPT_GRAB   (0x0040)
#  define OPT_INT    (0x0080)
#  define OPT_FLOAT  (0x0100)
#  define OPT_SUBTITLE (0x0200)
#  define OPT_FUNC2  (0x0400)
#  define OPT_INT64  (0x0800)
    union {
        void (*func_arg)(const char *); //FIXME passing error code as int return would be nicer then exit() in the func
        int *int_arg;
        char **str_arg;
        float *float_arg;
        int (*func2_arg)(const char *, const char *);
        int64_t *int64_arg;
    } u;
    const char *help;
    const char *argname;
} OptionDef;

void show_help_options(const OptionDef *options, const char *msg, int mask, int value);

/*!
 * Parses the command line arguments.
 * \param options Array with the definitions required to interpret every
 * option of the form: -<option_name> [<argument>]
 * \param parse_arg_function Name of the function called to process every
 * argument without a leading option name flag. NULL if such arguments do
 * not have to be processed.
 */
void parse_options(int argc, char **argv, const OptionDef *options,
                   void (* parse_arg_function)(const char*));

void print_error(const char *filename, int err);

void show_help_options(const OptionDef *options, const char *msg, int mask, int value)
{
    const OptionDef *po;
    int first;

    first = 1;
    for(po = options; po->name != NULL; po++) {
        char buf[64];
        if ((po->flags & mask) == value) {
            if (first) {
                printf("%s", msg);
                first = 0;
            }
            av_strlcpy(buf, po->name, sizeof(buf));
            if (po->flags & HAS_ARG) {
                av_strlcat(buf, " ", sizeof(buf));
                av_strlcat(buf, po->argname, sizeof(buf));
            }
            printf("-%-17s  %s\n", buf, po->help);
        }
    }
}

static const OptionDef* find_option(const OptionDef *po, const char *name){
    while (po->name != NULL) {
        if (!strcmp(name, po->name))
            break;
        po++;
    }
    return po;
}

void parse_options(int argc, char **argv, const OptionDef *options,
                   void (* parse_arg_function)(const char*))
{
    const char *opt, *arg;
    int optindex, handleoptions=1;
    const OptionDef *po;

    /* parse options */
    optindex = 1;
    while (optindex < argc) {
        opt = argv[optindex++];

        if (handleoptions && opt[0] == '-' && opt[1] != '\0') {
            if (opt[1] == '-' && opt[2] == '\0') {
                handleoptions = 0;
                continue;
            }
            po= find_option(options, opt + 1);
            if (!po->name)
                po= find_option(options, "default");
            if (!po->name) {
            unknown_opt:
                fprintf(stderr, "%s: unrecognized option '%s'\n", argv[0], opt);
                exit(1);
            }
            arg = NULL;
            if (po->flags & HAS_ARG) {
                arg = argv[optindex++];
                if (!arg) {
                    fprintf(stderr, "%s: missing argument for option '%s'\n", argv[0], opt);
                    exit(1);
                }
            }
            if (po->flags & OPT_STRING) {
                char *str;
                str = av_strdup(arg);
                *po->u.str_arg = str;
            } else if (po->flags & OPT_BOOL) {
                *po->u.int_arg = 1;
            } else if (po->flags & OPT_INT) {
                *po->u.int_arg = atoi(arg);
            } else if (po->flags & OPT_INT64) {
                *po->u.int64_arg = strtoll(arg, (char **)NULL, 10);
            } else if (po->flags & OPT_FLOAT) {
                *po->u.float_arg = atof(arg);
            } else if (po->flags & OPT_FUNC2) {
                if(po->u.func2_arg(opt+1, arg)<0)
                    goto unknown_opt;
            } else {
                po->u.func_arg(arg);
            }
        } else {
            if (parse_arg_function)
                parse_arg_function(opt);
        }
    }
}

void print_error(const char *filename, int err)
{
    switch(err) {
    case AVERROR_NUMEXPECTED:
        fprintf(stderr, "%s: Incorrect image filename syntax.\n"
                "Use '%%d' to specify the image number:\n"
                "  for img1.jpg, img2.jpg, ..., use 'img%%d.jpg';\n"
                "  for img001.jpg, img002.jpg, ..., use 'img%%03d.jpg'.\n",
                filename);
        break;
    case AVERROR_INVALIDDATA:
        fprintf(stderr, "%s: Error while parsing header\n", filename);
        break;
    case AVERROR_NOFMT:
        fprintf(stderr, "%s: Unknown format\n", filename);
        break;
    case AVERROR(EIO):
        fprintf(stderr, "%s: I/O error occured\n"
                "Usually that means that input file is truncated and/or corrupted.\n",
                filename);
        break;
    case AVERROR(ENOMEM):
        fprintf(stderr, "%s: memory allocation error occured\n", filename);
        break;
    case AVERROR(ENOENT):
        fprintf(stderr, "%s: no such file or directory\n", filename);
        break;
    default:
        fprintf(stderr, "%s: Error while opening file\n", filename);
        break;
    }
}

/* ========================================================================= */

static const char g_program_name[] = "FFplay";
static const int g_program_birth_year = 2003;

//#define DEBUG_SYNC

#  define MAX_VIDEOQ_SIZE (5 * 256 * 1024)
#define MAX_AUDIOQ_SIZE (5 * 16 * 1024)
#  define MAX_SUBTITLEQ_SIZE (5 * 16 * 1024)

/* SDL audio buffer size, in samples. Should be small to have precise
   A/V sync as SDL does not have hardware buffer fullness info. */
#  define SDL_AUDIO_BUFFER_SIZE 1024

/* no AV sync correction is done if below the AV sync threshold */
#  define AV_SYNC_THRESHOLD 0.01
/* no AV correction is done if too big error */
#  define AV_NOSYNC_THRESHOLD 10.0

/* maximum audio speed change to get correct sync */
#  define SAMPLE_CORRECTION_PERCENT_MAX 10

/* we use about AUDIO_DIFF_AVG_NB A-V differences to make the average */
#  define AUDIO_DIFF_AVG_NB   20

/* NOTE: the size must be big enough to compensate the hardware audio buffersize size */
#  define SAMPLE_ARRAY_SIZE (2*65536)

static int g_sws_flags = SWS_BICUBIC;

/*!
 * Packet Queue.
 */
typedef struct PacketQueue {
    AVPacketList *first_pkt, *last_pkt;
    int nb_packets;
    int size;
    int abort_request;
    pthread_mutex_t mutex;
    pthread_cond_t cond;
} PacketQueue;

#  define VIDEO_PICTURE_QUEUE_SIZE 1
#define SUBPICTURE_QUEUE_SIZE 4

/*!
 * Video Picture.
 */
typedef struct VideoPicture {
    double pts;                                  /**< presentation time stamp for this picture */
    SDL_Overlay *bmp;
    int width, height;		/**< source height & width */
    int allocated;
} VideoPicture;

/*!
 * Sub Picture.
 */
typedef struct SubPicture {
    double pts; /* presentation time stamp for this picture */
    AVSubtitle sub;
} SubPicture;

/*!
 * Synchronization Type Code.
 */
enum {
    AV_SYNC_AUDIO_MASTER,		/**< default choice */
    AV_SYNC_VIDEO_MASTER,
    AV_SYNC_EXTERNAL_CLOCK,	/**< synchronize to an external clock */
};

/*!
 * Video State.
 */
typedef struct VideoState {
    pthread_t parse_tid;
    pthread_t video_tid;
    AVInputFormat *iformat;
    int no_background;
    int abort_request;
    int paused;			/**< Paused state. */
    int last_paused;
    int seek_req;
    int seek_flags;
    int64_t seek_pos;		/**< Seek Position. */
    AVFormatContext *ic;
    int dtg_active_format;

    int av_sync_type;
    double external_clock;	/**< external clock base */
    int64_t external_clock_time;

    double audio_clock;		/**< Audio Clock. */
    double audio_diff_cum; /**< Cumulative Difference. Used for AV difference average computation */
    double audio_diff_avg_coef;
    double audio_diff_threshold;	/**< Difference Threshold. */
    int audio_diff_avg_count;

    int audio_stream_index;		/**< Audio Stream index. */
    AVStream *audio_stream;		/**< Audio stream. */

    PacketQueue audioq;		/**< Audio Packet Queue. */
    int audio_hw_buf_size;

    /* samples output by the codec. we reserve more space for avsync
       compensation */
    uint8_t audio_buf[(AVCODEC_MAX_AUDIO_FRAME_SIZE * 3) / 2] __attribute__ ((aligned(16)));
    unsigned int audio_buf_size; /* in bytes */
    int audio_buf_index;	       /* in bytes */

    AVPacket audio_pkt;	       /**< Audio Packet. */
    uint8_t *audio_pkt_data;	/**< Audio Packet Data. */
    int audio_pkt_size;		/**< Audio Packet Size. */

    uint show_audio_flag;		/**< if true, display audio samples */
    int16_t sample_array[SAMPLE_ARRAY_SIZE];
    int sample_array_index;
    int last_i_start;

    pthread_t subtitle_tid;
    int subtitle_stream_index;
    int subtitle_stream_changed;
    AVStream *subtitle_stream;
    PacketQueue subtitleq;

    SubPicture subpQ[SUBPICTURE_QUEUE_SIZE];
    int subpQ_size, subpQ_rindex, subpQ_windex;
    pthread_mutex_t subpQ_mutex;
    pthread_cond_t subpQ_cond;

    double frame_timer;		/**< Frame Timer. */
    double frame_last_pts;
    double frame_last_delay;
    double video_clock; /**< pts of last decoded frame / predicted pts of next decoded frame */

    int video_stream_index;	/**< Video Stream Index. */
    AVStream *video_stream;	/**< Video Stream. */

    PacketQueue videoq;		/**< Video Packet Queue. */
    double video_current_pts; /**< Current displayed pts (different from video_clock if frame fifos are used) */
    int64_t video_current_pts_time; /**< Time (av_gettime) at which we updated video_current_pts - used to have running video pts */
    VideoPicture pictQ[VIDEO_PICTURE_QUEUE_SIZE];
    int pictQ_size, pictQ_rindex, pictQ_windex;
    pthread_mutex_t pictQ_mutex;
    pthread_cond_t pictQ_cond;

    //    QETimer *video_timer;
    char filename[1024];
    int width, height, xleft, ytop;

    struct SwsContext *img_convert_ctx;
    int64_t last_time;
} VideoState;

void show_help(void);
static int audio_write_get_buf_size(VideoState *is);

/* options specified by the user */
static AVInputFormat *g_file_iformat;
static const char *g_input_filename;
static int g_fs_screen_width;
static int g_fs_screen_height;
static int g_screen_width = 0;
static int g_screen_height = 0;
static int g_frame_width = 0;
static int g_frame_height = 0;
static enum PixelFormat g_frame_pix_fmt = PIX_FMT_NONE;
static int g_audio_disable;
static int g_video_disable;
static int g_wanted_audio_stream= 0;
static int g_wanted_video_stream= 0;
static int g_seek_by_bytes;
static int g_display_disable;
static int g_show_status;
static int g_av_sync_type = AV_SYNC_AUDIO_MASTER;
static int64_t g_start_time = AV_NOPTS_VALUE;
static int debug = 0;
static int debug_mv = 0;
static int step = 0;
static int thread_count = 1;
static int workaround_bugs = 1;
static int fast = 0;
static int genpts = 0;
static int lowres = 0;
static int idct = FF_IDCT_AUTO;
static enum AVDiscard skip_frame= AVDISCARD_DEFAULT;
static enum AVDiscard skip_idct= AVDISCARD_DEFAULT;
static enum AVDiscard skip_loop_filter= AVDISCARD_DEFAULT;
static int error_resilience = FF_ER_CAREFUL;
static int error_concealment = 3;
static int decoder_reorder_pts= 0;

/* current context */
static int is_full_screen = 0;
static VideoState * cur_stream = NULL;
static int64_t audio_callback_time = 0;

AVPacket g_flush_pkt;

#  define FF_ALLOC_EVENT   (SDL_USEREVENT)
#define FF_REFRESH_EVENT (SDL_USEREVENT + 1)
#  define FF_QUIT_EVENT    (SDL_USEREVENT + 2)

SDL_Surface * screen = NULL;
static int g_screen_initialized_flag = 0;

/*!
 * Packet Queue Initialize.
 */
static void packet_queue_init(PacketQueue *q)
{
    memset(q, 0, sizeof(PacketQueue));
    pthread_mutex_init(&q->mutex, 0);
    pthread_cond_init(&q->cond, NULL);
}

/*!
 * Packet Queue Flush.
 */
static void packet_queue_flush(PacketQueue *q)
{
    AVPacketList *pkt, *pkt1;

    pthread_mutex_lock(&q->mutex);
    for(pkt = q->first_pkt; pkt != NULL; pkt = pkt1) {
        pkt1 = pkt->next;
        av_free_packet(&pkt->pkt);
        av_freep(&pkt);
    }
    q->last_pkt = NULL;
    q->first_pkt = NULL;
    q->nb_packets = 0;
    q->size = 0;
    pthread_mutex_unlock(&q->mutex);
}

/*!
 * Packet Queue End.
 */
static void packet_queue_end(PacketQueue *q)
{
    packet_queue_flush(q);
    pthread_mutex_destroy(&q->mutex);
    pthread_cond_destroy(&q->cond);
}

/*!
 * Packet Queue Put.
 */
static int packet_queue_put(PacketQueue *q, AVPacket *pkt)
{
    AVPacketList *pkt1;

    /* duplicate the packet */
    if (pkt!=&g_flush_pkt && av_dup_packet(pkt) < 0)
        return -1;

    pkt1 = av_malloc(sizeof(AVPacketList));
    if (!pkt1)
        return -1;
    pkt1->pkt = *pkt;
    pkt1->next = NULL;

    {
        pthread_mutex_lock(&q->mutex);
        if (!q->last_pkt)
            q->first_pkt = pkt1;
        else
            q->last_pkt->next = pkt1;
        q->last_pkt = pkt1;
        q->nb_packets++;
        q->size += pkt1->pkt.size;
        /* XXX: should duplicate packet data in DV case */
        pthread_cond_signal(&q->cond);
        pthread_mutex_unlock(&q->mutex);
    }
    return 0;
}

/*!
 * Packet Queue Abort.
 */
static void packet_queue_abort(PacketQueue *q)
{
    pthread_mutex_lock(&q->mutex);
    q->abort_request = 1;
    pthread_cond_signal(&q->cond);
    pthread_mutex_unlock(&q->mutex);
}

/*!
 * Packet Queue Get.
 * \return < 0 if aborted, 0 if no packet and > 0 if packet.
 */
static int packet_queue_get(PacketQueue *q, AVPacket *pkt, int block)
{
    AVPacketList *pkt1;
    int ret;

    pthread_mutex_lock(&q->mutex);

    for(;;) {
        if (q->abort_request) {
            ret = -1;
            break;
        }

        pkt1 = q->first_pkt;
        if (pkt1) {
            q->first_pkt = pkt1->next;
            if (!q->first_pkt)
                q->last_pkt = NULL;
            q->nb_packets--;
            q->size -= pkt1->pkt.size;
            *pkt = pkt1->pkt;
            av_free(pkt1);
            ret = 1;
            break;
        } else if (!block) {
            ret = 0;
            break;
        } else {
            pthread_cond_wait(&q->cond, &q->mutex);
        }
    }
    pthread_mutex_unlock(&q->mutex);
    return ret;
}

static inline void fill_rectangle(SDL_Surface *screen,
                                  int x, int y, int w, int h, int color)
{
    SDL_Rect rect;
    rect.x = x;
    rect.y = y;
    rect.w = w;
    rect.h = h;
    SDL_FillRect(screen, &rect, color);
}

/*!
 * Draw only the border of a rectangle.
 */
void fill_border(VideoState *s, int x, int y, int w, int h, int color)
{
    int w1, w2, h1, h2;

    /* fill the background */
    w1 = x;
    if (w1 < 0)
        w1 = 0;
    w2 = s->width - (x + w);
    if (w2 < 0)
        w2 = 0;
    h1 = y;
    if (h1 < 0)
        h1 = 0;
    h2 = s->height - (y + h);
    if (h2 < 0)
        h2 = 0;
    fill_rectangle(screen,
                   s->xleft, s->ytop,
                   w1, s->height,
                   color);
    fill_rectangle(screen,
                   s->xleft + s->width - w2, s->ytop,
                   w2, s->height,
                   color);
    fill_rectangle(screen,
                   s->xleft + w1, s->ytop,
                   s->width - w1 - w2, h1,
                   color);
    fill_rectangle(screen,
                   s->xleft + w1, s->ytop + s->height - h2,
                   s->width - w1 - w2, h2,
                   color);
}


#  define SCALEBITS 10
#define ONE_HALF  (1 << (SCALEBITS - 1))
#  define FIX(x)    ((int) ((x) * (1<<SCALEBITS) + 0.5))

#define RGB_TO_Y_CCIR(r, g, b)                                          \
    ((FIX(0.29900*219.0/255.0) * (r) + FIX(0.58700*219.0/255.0) * (g) +	\
      FIX(0.11400*219.0/255.0) * (b) + (ONE_HALF + (16 << SCALEBITS))) >> SCALEBITS)

#  define RGB_TO_U_CCIR(r1, g1, b1, shift)                              \
    (((- FIX(0.16874*224.0/255.0) * r1 - FIX(0.33126*224.0/255.0) * g1 + \
       FIX(0.50000*224.0/255.0) * b1 + (ONE_HALF << shift) - 1) >> (SCALEBITS + shift)) + 128)

#  define RGB_TO_V_CCIR(r1, g1, b1, shift)                              \
    (((FIX(0.50000*224.0/255.0) * r1 - FIX(0.41869*224.0/255.0) * g1 -	\
       FIX(0.08131*224.0/255.0) * b1 + (ONE_HALF << shift) - 1) >> (SCALEBITS + shift)) + 128)

#  define ALPHA_BLEND(a, oldp, newp, s)                         \
    ((((oldp << s) * (255 - (a))) + (newp * (a))) / (255 << s))

#  define RGBA_IN(r, g, b, a, s)                        \
    {                                                   \
        unsigned int v = ((const uint32_t *)(s))[0];	\
        a = (v >> 24) & 0xff;                           \
        r = (v >> 16) & 0xff;                           \
        g = (v >> 8) & 0xff;                            \
        b = v & 0xff;                                   \
    }

#  define YUVA_IN(y, u, v, a, s, pal)                                   \
    {                                                                   \
        unsigned int val = ((const uint32_t *)(pal))[*(const uint8_t*)(s)]; \
        a = (val >> 24) & 0xff;                                         \
        y = (val >> 16) & 0xff;                                         \
        u = (val >> 8) & 0xff;                                          \
        v = val & 0xff;                                                 \
    }

#  define YUVA_OUT(d, y, u, v, a)                                       \
    {                                                                   \
        ((uint32_t *)(d))[0] = (a << 24) | (y << 16) | (u << 8) | v;	\
    }


#  define BPP 1

static void blend_subrect(AVPicture *dst, const AVSubtitleRect *rect, int imgw, int imgh)
{
    int wrap, wrap3, width2, skip2;
    int y, u, v, a, u1, v1, a1, w, h;
    uint8_t *lum, *cb, *cr;
    const uint8_t *p;
    const uint32_t *pal;
    int dstx, dsty, dstw, dsth;

    dstx = FFMIN(FFMAX(rect->x, 0), imgw);
    dstw = FFMIN(FFMAX(rect->w, 0), imgw - dstx);
    dsty = FFMIN(FFMAX(rect->y, 0), imgh);
    dsth = FFMIN(FFMAX(rect->h, 0), imgh - dsty);
    lum = dst->data[0] + dsty * dst->linesize[0];
    cb = dst->data[1] + (dsty >> 1) * dst->linesize[1];
    cr = dst->data[2] + (dsty >> 1) * dst->linesize[2];

    width2 = (dstw + 1) >> 1;
    skip2 = dstx >> 1;
    wrap = dst->linesize[0];
    wrap3 = rect->linesize;
    p = rect->bitmap;
    pal = rect->rgba_palette;  /* Now in YCrCb! */

    if (dsty & 1) {
        lum += dstx;
        cb += skip2;
        cr += skip2;

        if (dstx & 1) {
            YUVA_IN(y, u, v, a, p, pal);
            lum[0] = ALPHA_BLEND(a, lum[0], y, 0);
            cb[0] = ALPHA_BLEND(a >> 2, cb[0], u, 0);
            cr[0] = ALPHA_BLEND(a >> 2, cr[0], v, 0);
            cb++;
            cr++;
            lum++;
            p += BPP;
        }
        for(w = dstw - (dstx & 1); w >= 2; w -= 2) {
            YUVA_IN(y, u, v, a, p, pal);
            u1 = u;
            v1 = v;
            a1 = a;
            lum[0] = ALPHA_BLEND(a, lum[0], y, 0);

            YUVA_IN(y, u, v, a, p + BPP, pal);
            u1 += u;
            v1 += v;
            a1 += a;
            lum[1] = ALPHA_BLEND(a, lum[1], y, 0);
            cb[0] = ALPHA_BLEND(a1 >> 2, cb[0], u1, 1);
            cr[0] = ALPHA_BLEND(a1 >> 2, cr[0], v1, 1);
            cb++;
            cr++;
            p += 2 * BPP;
            lum += 2;
        }
        if (w) {
            YUVA_IN(y, u, v, a, p, pal);
            lum[0] = ALPHA_BLEND(a, lum[0], y, 0);
            cb[0] = ALPHA_BLEND(a >> 2, cb[0], u, 0);
            cr[0] = ALPHA_BLEND(a >> 2, cr[0], v, 0);
        }
        p += wrap3 + (wrap3 - dstw * BPP);
        lum += wrap + (wrap - dstw - dstx);
        cb += dst->linesize[1] - width2 - skip2;
        cr += dst->linesize[2] - width2 - skip2;
    }
    for(h = dsth - (dsty & 1); h >= 2; h -= 2) {
        lum += dstx;
        cb += skip2;
        cr += skip2;

        if (dstx & 1) {
            YUVA_IN(y, u, v, a, p, pal);
            u1 = u;
            v1 = v;
            a1 = a;
            lum[0] = ALPHA_BLEND(a, lum[0], y, 0);
            p += wrap3;
            lum += wrap;
            YUVA_IN(y, u, v, a, p, pal);
            u1 += u;
            v1 += v;
            a1 += a;
            lum[0] = ALPHA_BLEND(a, lum[0], y, 0);
            cb[0] = ALPHA_BLEND(a1 >> 2, cb[0], u1, 1);
            cr[0] = ALPHA_BLEND(a1 >> 2, cr[0], v1, 1);
            cb++;
            cr++;
            p += -wrap3 + BPP;
            lum += -wrap + 1;
        }
        for(w = dstw - (dstx & 1); w >= 2; w -= 2) {
            YUVA_IN(y, u, v, a, p, pal);
            u1 = u;
            v1 = v;
            a1 = a;
            lum[0] = ALPHA_BLEND(a, lum[0], y, 0);

            YUVA_IN(y, u, v, a, p, pal);
            u1 += u;
            v1 += v;
            a1 += a;
            lum[1] = ALPHA_BLEND(a, lum[1], y, 0);
            p += wrap3;
            lum += wrap;

            YUVA_IN(y, u, v, a, p, pal);
            u1 += u;
            v1 += v;
            a1 += a;
            lum[0] = ALPHA_BLEND(a, lum[0], y, 0);

            YUVA_IN(y, u, v, a, p, pal);
            u1 += u;
            v1 += v;
            a1 += a;
            lum[1] = ALPHA_BLEND(a, lum[1], y, 0);

            cb[0] = ALPHA_BLEND(a1 >> 2, cb[0], u1, 2);
            cr[0] = ALPHA_BLEND(a1 >> 2, cr[0], v1, 2);

            cb++;
            cr++;
            p += -wrap3 + 2 * BPP;
            lum += -wrap + 2;
        }
        if (w) {
            YUVA_IN(y, u, v, a, p, pal);
            u1 = u;
            v1 = v;
            a1 = a;
            lum[0] = ALPHA_BLEND(a, lum[0], y, 0);
            p += wrap3;
            lum += wrap;
            YUVA_IN(y, u, v, a, p, pal);
            u1 += u;
            v1 += v;
            a1 += a;
            lum[0] = ALPHA_BLEND(a, lum[0], y, 0);
            cb[0] = ALPHA_BLEND(a1 >> 2, cb[0], u1, 1);
            cr[0] = ALPHA_BLEND(a1 >> 2, cr[0], v1, 1);
            cb++;
            cr++;
            p += -wrap3 + BPP;
            lum += -wrap + 1;
        }
        p += wrap3 + (wrap3 - dstw * BPP);
        lum += wrap + (wrap - dstw - dstx);
        cb += dst->linesize[1] - width2 - skip2;
        cr += dst->linesize[2] - width2 - skip2;
    }
    /* handle odd height */
    if (h) {
        lum += dstx;
        cb += skip2;
        cr += skip2;

        if (dstx & 1) {
            YUVA_IN(y, u, v, a, p, pal);
            lum[0] = ALPHA_BLEND(a, lum[0], y, 0);
            cb[0] = ALPHA_BLEND(a >> 2, cb[0], u, 0);
            cr[0] = ALPHA_BLEND(a >> 2, cr[0], v, 0);
            cb++;
            cr++;
            lum++;
            p += BPP;
        }
        for(w = dstw - (dstx & 1); w >= 2; w -= 2) {
            YUVA_IN(y, u, v, a, p, pal);
            u1 = u;
            v1 = v;
            a1 = a;
            lum[0] = ALPHA_BLEND(a, lum[0], y, 0);

            YUVA_IN(y, u, v, a, p + BPP, pal);
            u1 += u;
            v1 += v;
            a1 += a;
            lum[1] = ALPHA_BLEND(a, lum[1], y, 0);
            cb[0] = ALPHA_BLEND(a1 >> 2, cb[0], u, 1);
            cr[0] = ALPHA_BLEND(a1 >> 2, cr[0], v, 1);
            cb++;
            cr++;
            p += 2 * BPP;
            lum += 2;
        }
        if (w) {
            YUVA_IN(y, u, v, a, p, pal);
            lum[0] = ALPHA_BLEND(a, lum[0], y, 0);
            cb[0] = ALPHA_BLEND(a >> 2, cb[0], u, 0);
            cr[0] = ALPHA_BLEND(a >> 2, cr[0], v, 0);
        }
    }
}

static void free_subpicture(SubPicture *sp)
{
    uint i;
    for (i = 0; i < sp->sub.num_rects; i++) {
        av_free(sp->sub.rects[i].bitmap);
        av_free(sp->sub.rects[i].rgba_palette);
    }
    av_free(sp->sub.rects);
    memset(&sp->sub, 0, sizeof(AVSubtitle));
}

static void video_display_image(VideoState *is)
{
    VideoPicture *vp;
    SubPicture *sp;
    AVPicture pict;
    float aspect_ratio;
    int width, height, x, y;
    SDL_Rect rect;
    uint i;

    vp = &is->pictQ[is->pictQ_rindex];
    if (vp->bmp) {
        /* XXX: use variable in the frame */
        if (is->video_stream->codec->sample_aspect_ratio.num == 0)
            aspect_ratio = 0;
        else
            aspect_ratio = av_q2d(is->video_stream->codec->sample_aspect_ratio)
                * is->video_stream->codec->width / is->video_stream->codec->height;;
        if (aspect_ratio <= 0.0)
            aspect_ratio = (float)is->video_stream->codec->width /
                (float)is->video_stream->codec->height;
        /* if an active format is indicated, then it overrides the
           mpeg format */
#  if 0
        if (is->video_stream->codec->dtg_active_format != is->dtg_active_format) {
            is->dtg_active_format = is->video_stream->codec->dtg_active_format;
            printf("dtg_active_format=%d\n", is->dtg_active_format);
        }
#  endif
#if 0
        switch(is->video_stream->codec->dtg_active_format) {
        case FF_DTG_AFD_SAME: break;
        default: /* nothing to do */ break;
        case FF_DTG_AFD_4_3: aspect_ratio = 4.0 / 3.0; break;
        case FF_DTG_AFD_16_9: aspect_ratio = 16.0 / 9.0; break;
        case FF_DTG_AFD_14_9: aspect_ratio = 14.0 / 9.0; break;
        case FF_DTG_AFD_4_3_SP_14_9: aspect_ratio = 14.0 / 9.0; break;
        case FF_DTG_AFD_16_9_SP_14_9: aspect_ratio = 14.0 / 9.0; break;
        case FF_DTG_AFD_SP_4_3: aspect_ratio = 4.0 / 3.0; break;
        }
#endif

        if (is->subtitle_stream) {
            if (is->subpQ_size > 0) {
                sp = &is->subpQ[is->subpQ_rindex];
                if (vp->pts >= sp->pts + ((float) sp->sub.start_display_time / 1000)) {
                    SDL_LockYUVOverlay (vp->bmp);
                    pict.data[0] = vp->bmp->pixels[0];
                    pict.data[1] = vp->bmp->pixels[2];
                    pict.data[2] = vp->bmp->pixels[1];
                    pict.linesize[0] = vp->bmp->pitches[0];
                    pict.linesize[1] = vp->bmp->pitches[2];
                    pict.linesize[2] = vp->bmp->pitches[1];
                    for (i = 0; i < sp->sub.num_rects; i++)
                        blend_subrect(&pict, &sp->sub.rects[i],
                                      vp->bmp->w, vp->bmp->h);
                    SDL_UnlockYUVOverlay (vp->bmp);
                }
            }
        }


        /* XXX: we suppose the screen has a 1.0 pixel ratio */
        height = is->height;
        width = ((int)rint(height * aspect_ratio)) & -3;
        if (width > is->width) {
            width = is->width;
            height = ((int)rint(width / aspect_ratio)) & -3;
        }
        x = (is->width - width) / 2;
        y = (is->height - height) / 2;
        if (!is->no_background) {
            /* fill the background */
            //            fill_border(is, x, y, width, height, QERGB(0x00, 0x00, 0x00));
        } else {
            is->no_background = 0;
        }
        rect.x = is->xleft + x;
        rect.y = is->ytop  + y;
        rect.w = width;
        rect.h = height;
        SDL_DisplayYUVOverlay(vp->bmp, &rect);
    } else {
#if 0
        fill_rectangle(screen,
                       is->xleft, is->ytop, is->width, is->height,
                       QERGB(0x00, 0x00, 0x00));
#endif
    }
}

static inline int compute_mod(int a, int b)
{
    a = a % b;
    if (a >= 0)
        return a;
    else
        return a + b;
}

static void video_display_audio(VideoState *s)
{
    int i, i_start, x, y1, y, ys, delay, n, nb_display_channels;
    int ch, channels, h, h2, bgcolor, fgcolor;
    int16_t time_diff;

    /* compute display index : center on currently output samples */
    channels = s->audio_stream->codec->channels;
    nb_display_channels = channels;
    if (!s->paused) {
        n = 2 * channels;
        delay = audio_write_get_buf_size(s);
        delay /= n;

        /* to be more precise, we take into account the time spent since
           the last buffer computation */
        if (audio_callback_time) {
            time_diff = av_gettime() - audio_callback_time;
            delay += (time_diff * s->audio_stream->codec->sample_rate) / 1000000;
        }

        delay -= s->width / 2;
        if (delay < s->width)
            delay = s->width;

        i_start= x = compute_mod(s->sample_array_index - delay * channels, SAMPLE_ARRAY_SIZE);

        h= INT_MIN;
        for(i=0; i<1000; i+=channels) {
            int idx= (SAMPLE_ARRAY_SIZE + x - i) % SAMPLE_ARRAY_SIZE;
            int a= s->sample_array[idx];
            int b= s->sample_array[(idx + 4*channels)%SAMPLE_ARRAY_SIZE];
            int c= s->sample_array[(idx + 5*channels)%SAMPLE_ARRAY_SIZE];
            int d= s->sample_array[(idx + 9*channels)%SAMPLE_ARRAY_SIZE];
            int score= a-d;
            if(h<score && (b^c)<0) {
                h= score;
                i_start= idx;
            }
        }

        s->last_i_start = i_start;
    } else {
        i_start = s->last_i_start;
    }

    bgcolor = SDL_MapRGB(screen->format, 0x00, 0x00, 0x00);
    fill_rectangle(screen,
                   s->xleft, s->ytop, s->width, s->height,
                   bgcolor);

    fgcolor = SDL_MapRGB(screen->format, 0xff, 0xff, 0xff);

    /* total height for one channel */
    h = s->height / nb_display_channels;
    /* graph height / 2 */
    h2 = (h * 9) / 20;
    for(ch = 0;ch < nb_display_channels; ch++) {
        i = i_start + ch;
        y1 = s->ytop + ch * h + (h / 2); /* position of center line */
        for(x = 0; x < s->width; x++) {
            y = (s->sample_array[i] * h2) >> 15;
            if (y < 0) {
                y = -y;
                ys = y1 - y;
            } else {
                ys = y1;
            }
            fill_rectangle(screen,
                           s->xleft + x, ys, 1, y,
                           fgcolor);
            i += channels;
            if (i >= SAMPLE_ARRAY_SIZE)
                i -= SAMPLE_ARRAY_SIZE;
        }
    }

    fgcolor = SDL_MapRGB(screen->format, 0x00, 0x00, 0xff);

    for(ch = 1;ch < nb_display_channels; ch++) {
        y = s->ytop + ch * h;
        fill_rectangle(screen,
                       s->xleft, y, s->width, 1,
                       fgcolor);
    }
    SDL_UpdateRect(screen, s->xleft, s->ytop, s->width, s->height);
}

static int video_open(VideoState *is)
{
    int flags = SDL_HWSURFACE|SDL_ASYNCBLIT|SDL_HWACCEL;
    int w,h;

    if (is_full_screen) flags |= SDL_FULLSCREEN;
    else                flags |= SDL_RESIZABLE;

    if (is_full_screen && g_fs_screen_width) {
        w = g_fs_screen_width;
        h = g_fs_screen_height;
    } else if(!is_full_screen && g_screen_width) {
        w = g_screen_width;
        h = g_screen_height;
    }else if (is->video_stream && is->video_stream->codec->width) {
        w = is->video_stream->codec->width;
        h = is->video_stream->codec->height;
    } else {
        w = 640;
        h = 480;
    }

#ifndef __APPLE__
    screen = SDL_SetVideoMode(w, h, 0, flags);
#else
    /* setting bits_per_pixel = 0 or 32 causes blank video on OS X */
    screen = SDL_SetVideoMode(w, h, 24, flags);
#endif
    if (!screen) {
        fprintf(stderr, "SDL: could not set video mode - exiting\n");
        return -1;
    }
    SDL_WM_SetCaption("FFplay", "FFplay");

    is->width = screen->w;
    is->height = screen->h;
/*   is->width = is->video_stream->codec->width; */
/*   is->height = is->video_stream->codec->height; */

    g_screen_initialized_flag = 1;

    return 0;
}

/*!*
 * Display the current picture, if any.
 */
static void video_display_audio_video(VideoState *is)
{
    if(!g_screen_initialized_flag) video_open(cur_stream);
    if (is->audio_stream && is->show_audio_flag) video_display_audio(is);
    else if (is->video_stream) video_display_image(is);
}

static uint32_t sdl_refresh_timer_cb(uint32_t interval, void *opaque)
{
    SDL_Event event;
    event.type = FF_REFRESH_EVENT;
    event.user.data1 = opaque;
    SDL_PushEvent(&event);
    return 0; /* 0 means stop timer */
}

/*!
 * Schedule a video refresh in \p delay ms.
 */
static void schedule_refresh(VideoState *is, int delay)
{
    SDL_AddTimer(delay, sdl_refresh_timer_cb, is);
}

/*!
 * Get the current audio clock value.
 */
static double get_audio_clock(VideoState *is)
{
    double pts;
    int hw_buf_size, bytes_per_sec;
    pts = is->audio_clock;
    hw_buf_size = audio_write_get_buf_size(is);
    bytes_per_sec = 0;
    if (is->audio_stream) {
        bytes_per_sec = is->audio_stream->codec->sample_rate *
            2 * is->audio_stream->codec->channels;
    }
    if (bytes_per_sec)
        pts -= (double)hw_buf_size / bytes_per_sec;
    return pts;
}

/*!
 * Get the current video clock value
 */
static double get_video_clock(VideoState *is)
{
    double delta;
    if (is->paused) {
        delta = 0;
    } else {
        delta = (av_gettime() - is->video_current_pts_time) / 1000000.0;
    }
    return is->video_current_pts + delta;
}

/*!
 * Get the current external clock value
 */
static double get_external_clock(VideoState *is)
{
    int64_t ti;
    ti = av_gettime();
    return is->external_clock + ((ti - is->external_clock_time) * 1e-6);
}

/*!
 * Get the current master clock value
 */
static double get_master_clock(VideoState *is)
{
    double val;

    if (is->av_sync_type == AV_SYNC_VIDEO_MASTER) {
        if (is->video_stream)
            val = get_video_clock(is);
        else
            val = get_audio_clock(is);
    } else if (is->av_sync_type == AV_SYNC_AUDIO_MASTER) {
        if (is->audio_stream)
            val = get_audio_clock(is);
        else
            val = get_video_clock(is);
    } else {
        val = get_external_clock(is);
    }
    return val;
}

/*!
 * Seek in the stream.
 */
static void stream_seek(VideoState *is, int64_t pos, int rel)
{
    if (!is->seek_req) {
        is->seek_pos = pos;
        is->seek_flags = rel < 0 ? AVSEEK_FLAG_BACKWARD : 0;
        if (g_seek_by_bytes)
            is->seek_flags |= AVSEEK_FLAG_BYTE;
        is->seek_req = 1;
    }
}

/*!
 * Pause or resume the video.
 */
static void stream_pause(VideoState *is)
{
    is->paused = !is->paused;
    if (!is->paused) {
        is->video_current_pts = get_video_clock(is);
        is->frame_timer += (av_gettime() - is->video_current_pts_time) / 1000000.0;
    }
}

/*!
 * Called to display each frame.
 */
static void video_refresh_timer(void *opaque)
{
    VideoState *is = opaque;
    VideoPicture *vp;
    double actual_delay, delay, sync_threshold, ref_clock, diff;

    SubPicture *sp, *sp2;

    if (is->video_stream) {
        if (is->pictQ_size == 0) {
            /* if no picture, need to wait */
            schedule_refresh(is, 1);
        } else {
            /* dequeue the picture */
            vp = &is->pictQ[is->pictQ_rindex];

            /* update current video pts */
            is->video_current_pts = vp->pts;
            is->video_current_pts_time = av_gettime();

            /* compute nominal delay */
            delay = vp->pts - is->frame_last_pts;
            if (delay <= 0 || delay >= 2.0) {
                /* if incorrect delay, use previous one */
                delay = is->frame_last_delay;
            }
            is->frame_last_delay = delay;
            is->frame_last_pts = vp->pts;

            /* update delay to follow master synchronisation source */
            if (((is->av_sync_type == AV_SYNC_AUDIO_MASTER && is->audio_stream) ||
                 is->av_sync_type == AV_SYNC_EXTERNAL_CLOCK)) {
                /* if video is slave, we try to correct big delays by
                   duplicating or deleting a frame */
                ref_clock = get_master_clock(is);
                diff = vp->pts - ref_clock;

                /* skip or repeat frame. We take into account the
                   delay to compute the threshold. I still don't know
                   if it is the best guess */
                sync_threshold = AV_SYNC_THRESHOLD;
                if (delay > sync_threshold)
                    sync_threshold = delay;
                if (fabs(diff) < AV_NOSYNC_THRESHOLD) {
                    if (diff <= -sync_threshold)
                        delay = 0;
                    else if (diff >= sync_threshold)
                        delay = 2 * delay;
                }
            }

            is->frame_timer += delay;
            /* compute the REAL delay (we need to do that to avoid
               long term errors */
            actual_delay = is->frame_timer - (av_gettime() / 1000000.0);
            if (actual_delay < 0.010) {
                /* XXX: should skip picture */
                actual_delay = 0.010;
            }
            /* launch timer for next picture */
            schedule_refresh(is, (int)(actual_delay * 1000 + 0.5));

#if defined(DEBUG_SYNC)
            printf("video: delay=%0.3f actual_delay=%0.3f pts=%0.3f A-V=%f\n",
                   delay, actual_delay, vp->pts, -diff);
#endif

            if(is->subtitle_stream) {
                if (is->subtitle_stream_changed) {
                    pthread_mutex_lock(&is->subpQ_mutex);

                    while (is->subpQ_size) {
                        free_subpicture(&is->subpQ[is->subpQ_rindex]);

                        /* update queue size and signal for next picture */
                        if (++is->subpQ_rindex == SUBPICTURE_QUEUE_SIZE)
                            is->subpQ_rindex = 0;

                        is->subpQ_size--;
                    }
                    is->subtitle_stream_changed = 0;

                    pthread_cond_signal(&is->subpQ_cond);
                    pthread_mutex_unlock(&is->subpQ_mutex);
                } else {
                    if (is->subpQ_size > 0) {
                        sp = &is->subpQ[is->subpQ_rindex];

                        if (is->subpQ_size > 1)
                            sp2 = &is->subpQ[(is->subpQ_rindex + 1) % SUBPICTURE_QUEUE_SIZE];
                        else
                            sp2 = NULL;

                        if ((is->video_current_pts > (sp->pts + ((float) sp->sub.end_display_time / 1000)))
                            || (sp2 && is->video_current_pts > (sp2->pts + ((float) sp2->sub.start_display_time / 1000))))
                        {
                            free_subpicture(sp);

                            /* update queue size and signal for next picture */
                            if (++is->subpQ_rindex == SUBPICTURE_QUEUE_SIZE)
                                is->subpQ_rindex = 0;

                            pthread_mutex_lock(&is->subpQ_mutex);
                            is->subpQ_size--;
                            pthread_cond_signal(&is->subpQ_cond);
                            pthread_mutex_unlock(&is->subpQ_mutex);
                        }
                    }
                }
            }

            /* display picture */
            video_display_audio_video(is);

            /* update queue size and signal for next picture */
            if (++is->pictQ_rindex == VIDEO_PICTURE_QUEUE_SIZE)
                is->pictQ_rindex = 0;

            pthread_mutex_lock(&is->pictQ_mutex);
            is->pictQ_size--;
            pthread_cond_signal(&is->pictQ_cond);
            pthread_mutex_unlock(&is->pictQ_mutex);
        }
    } else if (is->audio_stream) {
        /* draw the next audio frame */

        schedule_refresh(is, 40);

        /* if only audio stream, then display the audio bars (better
           than nothing, just to test the implementation */

        /* display picture */
        video_display_audio_video(is);
    } else {
        schedule_refresh(is, 100);
    }
    if (g_show_status) {
        int64_t cur_time;
        int aqsize, vqsize, sqsize;
        double av_diff;

        cur_time = av_gettime();
        if (!is->last_time || (cur_time - is->last_time) >= 500 * 1000) {
            aqsize = 0;
            vqsize = 0;
            sqsize = 0;
            if (is->audio_stream)
                aqsize = is->audioq.size;
            if (is->video_stream)
                vqsize = is->videoq.size;
            if (is->subtitle_stream)
                sqsize = is->subtitleq.size;
            av_diff = 0;
            if (is->audio_stream && is->video_stream)
                av_diff = get_audio_clock(is) - get_video_clock(is);
            printf("%7.2f A-V:%7.3f aq=%5dKB vq=%5dKB sq=%5dB    \r",
                   get_master_clock(is), av_diff, aqsize / 1024, vqsize / 1024, sqsize);
            fflush(stdout);
            is->last_time = cur_time;
        }
    }
}

/*!
 * Allocate a picture (needs to do that in main thread to avoid
 * potential locking problems
 */
static void alloc_picture(void *opaque)
{
    VideoState *is = opaque;
    VideoPicture *vp;

    vp = &is->pictQ[is->pictQ_windex];

    if (vp->bmp) SDL_FreeYUVOverlay(vp->bmp);

#if 0
    /* XXX: use generic function */
    /* XXX: disable overlay if no hardware acceleration or if RGB format */
    switch(is->video_stream->codec->pix_fmt) {
    case PIX_FMT_YUV420P:
    case PIX_FMT_YUV422P:
    case PIX_FMT_YUV444P:
    case PIX_FMT_YUYV422:
    case PIX_FMT_YUV410P:
    case PIX_FMT_YUV411P:
        is_yuv = 1;
        break;
    default:
        is_yuv = 0;
        break;
    }
#endif
    vp->bmp = SDL_CreateYUVOverlay(is->video_stream->codec->width,
                                   is->video_stream->codec->height,
                                   SDL_YV12_OVERLAY,
                                   screen);
    vp->width = is->video_stream->codec->width;
    vp->height = is->video_stream->codec->height;

    pthread_mutex_lock(&is->pictQ_mutex);
    vp->allocated = 1;
    pthread_cond_signal(&is->pictQ_cond);
    pthread_mutex_unlock(&is->pictQ_mutex);
}

/*!
 * Queue (Up) Picture.
 * \param pts the dts of the pkt / pts of the frame and guessed if not known.
 */
static int queue_picture(VideoState *is, AVFrame *src_frame, double pts)
{
    VideoPicture *vp;
    int dst_pix_fmt;
    AVPicture pict;

    /* wait until we have space to put a new picture */
    pthread_mutex_lock(&is->pictQ_mutex);
    while (is->pictQ_size >= VIDEO_PICTURE_QUEUE_SIZE &&
           !is->videoq.abort_request) {
        pthread_cond_wait(&is->pictQ_cond, &is->pictQ_mutex);
    }
    pthread_mutex_unlock(&is->pictQ_mutex);

    if (is->videoq.abort_request)
        return -1;

    vp = &is->pictQ[is->pictQ_windex];

    /* alloc or resize hardware picture buffer */
    if (!vp->bmp ||
        vp->width != is->video_stream->codec->width ||
        vp->height != is->video_stream->codec->height) {
        SDL_Event event;

        vp->allocated = 0;

        /* the allocation must be done in the main thread to avoid
           locking problems */
        event.type = FF_ALLOC_EVENT;
        event.user.data1 = is;
        SDL_PushEvent(&event);

        /* wait until the picture is allocated */
        pthread_mutex_lock(&is->pictQ_mutex);
        while (!vp->allocated && !is->videoq.abort_request) {
            pthread_cond_wait(&is->pictQ_cond, &is->pictQ_mutex);
        }
        pthread_mutex_unlock(&is->pictQ_mutex);

        if (is->videoq.abort_request)
            return -1;
    }

    /* if the frame is not skipped, then display it */
    if (vp->bmp) {
        /* get a pointer on the bitmap */
        SDL_LockYUVOverlay(vp->bmp);

        dst_pix_fmt = PIX_FMT_YUV420P;
        pict.data[0] = vp->bmp->pixels[0];
        pict.data[1] = vp->bmp->pixels[2];
        pict.data[2] = vp->bmp->pixels[1];

        pict.linesize[0] = vp->bmp->pitches[0];
        pict.linesize[1] = vp->bmp->pitches[2];
        pict.linesize[2] = vp->bmp->pitches[1];
        is->img_convert_ctx = sws_getCachedContext(is->img_convert_ctx,
                                                   is->video_stream->codec->width,
                                                   is->video_stream->codec->height,
                                                   is->video_stream->codec->pix_fmt,
                                                   is->video_stream->codec->width,
                                                   is->video_stream->codec->height,
                                                   dst_pix_fmt, g_sws_flags, NULL, NULL, NULL);
        if (is->img_convert_ctx == NULL) {
            fprintf(stderr, "Cannot initialize the conversion context\n");
            exit(1);
        }
        sws_scale(is->img_convert_ctx, src_frame->data, src_frame->linesize,
                  0, is->video_stream->codec->height, pict.data, pict.linesize);
        /* update the bitmap content */
        SDL_UnlockYUVOverlay(vp->bmp);

        vp->pts = pts;

        /* now we can update the picture count */
        if (++is->pictQ_windex == VIDEO_PICTURE_QUEUE_SIZE)
            is->pictQ_windex = 0;
        pthread_mutex_lock(&is->pictQ_mutex);
        is->pictQ_size++;
        pthread_mutex_unlock(&is->pictQ_mutex);
    }
    return 0;
}

/*!
 * Compute the exact PTS for the picture if it is omitted in the stream
 * \param pts1 the dts of the pkt / pts of the frame
 */
static int output_picture2(VideoState *is, AVFrame *src_frame, double pts1)
{
    double frame_delay, pts;

    pts = pts1;

    if (pts != 0) {
        /* update video clock with pts, if present */
        is->video_clock = pts;
    } else {
        pts = is->video_clock;
    }
    /* update video clock for next frame */
    frame_delay = av_q2d(is->video_stream->codec->time_base);
    /* for MPEG2, the frame can be repeated, so we update the
       clock accordingly */
    frame_delay += src_frame->repeat_pict * (frame_delay * 0.5);
    is->video_clock += frame_delay;

#if defined(DEBUG_SYNC) && 0
    {
        int ftype;
        if (src_frame->pict_type == FF_B_TYPE)
            ftype = 'B';
        else if (src_frame->pict_type == FF_I_TYPE)
            ftype = 'I';
        else
            ftype = 'P';
        printf("frame_type=%c clock=%0.3f pts=%0.3f\n",
               ftype, pts, pts1);
    }
#endif
    return queue_picture(is, src_frame, pts);
}

static uint64_t global_video_pkt_pts= AV_NOPTS_VALUE;

static int my_get_buffer(struct AVCodecContext *c, AVFrame *pic)
{
    int ret= avcodec_default_get_buffer(c, pic);
    uint64_t *pts= av_malloc(sizeof(uint64_t));
    *pts= global_video_pkt_pts;
    pic->opaque= pts;
    return ret;
}

static void my_release_buffer(struct AVCodecContext *c, AVFrame *pic)
{
    if(pic) av_freep(&pic->opaque);
    avcodec_default_release_buffer(c, pic);
}

/* \todo REName to avcodec_decode_video_loop() */
static void * video_thread(void *arg)
{
    VideoState *is = arg;
    AVPacket pkt1, *pkt = &pkt1;
    int len1, got_picture;
    AVFrame *frame= avcodec_alloc_frame();
    double pts;

    while (1) {
        while (is->paused && !is->videoq.abort_request) { usleep(10e3); }
        if (packet_queue_get(&is->videoq, pkt, 1) < 0) { break; }

        if(pkt->data == g_flush_pkt.data) {
            avcodec_flush_buffers(is->video_stream->codec);
            continue;
        }

        /* NOTE: ipts is the PTS of the _first_ picture beginning in
           this packet, if any */
        global_video_pkt_pts= pkt->pts;
        len1 = avcodec_decode_video(is->video_stream->codec,
                                    frame, &got_picture,
                                    pkt->data, pkt->size);

        if ((decoder_reorder_pts ||
             pkt->dts == AV_NOPTS_VALUE)
            && frame->opaque && *(uint64_t*)frame->opaque != AV_NOPTS_VALUE)
            pts= *(uint64_t*)frame->opaque;
        else if(pkt->dts != AV_NOPTS_VALUE)
            pts= pkt->dts;
        else
            pts= 0;
        pts *= av_q2d(is->video_stream->time_base);

        //            if (len1 < 0)
        //                break;
        if (got_picture) {
            if (output_picture2(is, frame, pts) < 0)
                goto the_end;
        }
        av_free_packet(pkt);
        if (step)
            if (cur_stream)
                stream_pause(cur_stream);
    }
the_end:
    av_free(frame);
    return 0;
}

static void * subtitle_thread(void *arg)
{
    VideoState *is = arg;
    SubPicture *sp;
    AVPacket pkt1, *pkt = &pkt1;
    int len1, got_subtitle;
    double pts;
    uint i, j;
    int r, g, b, y, u, v, a;

    for(;;) {
        while (is->paused && !is->subtitleq.abort_request) {
            SDL_Delay(10);
        }
        if (packet_queue_get(&is->subtitleq, pkt, 1) < 0)
            break;

        if(pkt->data == g_flush_pkt.data) {
            avcodec_flush_buffers(is->subtitle_stream->codec);
            continue;
        }
        pthread_mutex_lock(&is->subpQ_mutex);
        while (is->subpQ_size >= SUBPICTURE_QUEUE_SIZE &&
               !is->subtitleq.abort_request) {
            pthread_cond_wait(&is->subpQ_cond, &is->subpQ_mutex);
        }
        pthread_mutex_unlock(&is->subpQ_mutex);

        if (is->subtitleq.abort_request)
            goto the_end;

        sp = &is->subpQ[is->subpQ_windex];

        /* NOTE: ipts is the PTS of the _first_ picture beginning in
           this packet, if any */
        pts = 0;
        if (pkt->pts != AV_NOPTS_VALUE)
            pts = av_q2d(is->subtitle_stream->time_base)*pkt->pts;

        len1 = avcodec_decode_subtitle(is->subtitle_stream->codec,
                                       &sp->sub, &got_subtitle,
                                       pkt->data, pkt->size);
        //            if (len1 < 0)
        //                break;
        if (got_subtitle && sp->sub.format == 0) {
            sp->pts = pts;

            for (i = 0; i < sp->sub.num_rects; i++) {
                for (j = 0; j < sp->sub.rects[i].nb_colors; j++) {
                    RGBA_IN(r, g, b, a, sp->sub.rects[i].rgba_palette + j);
                    y = RGB_TO_Y_CCIR(r, g, b);
                    u = RGB_TO_U_CCIR(r, g, b, 0);
                    v = RGB_TO_V_CCIR(r, g, b, 0);
                    YUVA_OUT(sp->sub.rects[i].rgba_palette + j, y, u, v, a);
                }
            }

            /* now we can update the picture count */
            if (++is->subpQ_windex == SUBPICTURE_QUEUE_SIZE)
                is->subpQ_windex = 0;
            pthread_mutex_lock(&is->subpQ_mutex);
            is->subpQ_size++;
            pthread_mutex_unlock(&is->subpQ_mutex);
        }
        av_free_packet(pkt);
        //        if (step)
        //            if (cur_stream)
        //                stream_pause(cur_stream);
    }
the_end:
    return 0;
}

/*!
 * Copy samples for viewing in editor window.
 */
static void update_sample_display(VideoState *is, short *samples, int samples_size)
{
    int size, len, channels;

    channels = is->audio_stream->codec->channels;

    size = samples_size / sizeof(short);
    while (size > 0) {
        len = SAMPLE_ARRAY_SIZE - is->sample_array_index;
        if (len > size)
            len = size;
        memcpy(is->sample_array + is->sample_array_index, samples, len * sizeof(short));
        samples += len;
        is->sample_array_index += len;
        if (is->sample_array_index >= SAMPLE_ARRAY_SIZE)
            is->sample_array_index = 0;
        size -= len;
    }
}

/*!
 * Return the new audio buffer size (samples can be added or deleted
 * to get better sync if video or external master clock)
 */
static int synchronize_audio(VideoState *is, short *samples,
                             int samples_size1, double pts)
{
    int n, samples_size;
    double ref_clock;

    n = 2 * is->audio_stream->codec->channels;
    samples_size = samples_size1;

    /* if not master, then we try to remove or add samples to correct the clock */
    if (((is->av_sync_type == AV_SYNC_VIDEO_MASTER && is->video_stream) ||
         is->av_sync_type == AV_SYNC_EXTERNAL_CLOCK)) {
        double diff, avg_diff;
        int wanted_size, min_size, max_size, nb_samples;

        ref_clock = get_master_clock(is);
        diff = get_audio_clock(is) - ref_clock;

        if (diff < AV_NOSYNC_THRESHOLD) {
            is->audio_diff_cum = diff + is->audio_diff_avg_coef * is->audio_diff_cum;
            if (is->audio_diff_avg_count < AUDIO_DIFF_AVG_NB) {
                /* not enough measures to have a correct estimate */
                is->audio_diff_avg_count++;
            } else {
                /* estimate the A-V difference */
                avg_diff = is->audio_diff_cum * (1.0 - is->audio_diff_avg_coef);

                if (fabs(avg_diff) >= is->audio_diff_threshold) {
                    wanted_size = samples_size + ((int)(diff * is->audio_stream->codec->sample_rate) * n);
                    nb_samples = samples_size / n;

                    min_size = ((nb_samples * (100 - SAMPLE_CORRECTION_PERCENT_MAX)) / 100) * n;
                    max_size = ((nb_samples * (100 + SAMPLE_CORRECTION_PERCENT_MAX)) / 100) * n;
                    if (wanted_size < min_size)
                        wanted_size = min_size;
                    else if (wanted_size > max_size)
                        wanted_size = max_size;

                    /* add or remove samples to correction the synchro */
                    if (wanted_size < samples_size) {
                        /* remove samples */
                        samples_size = wanted_size;
                    } else if (wanted_size > samples_size) {
                        uint8_t *samples_end, *q;
                        int nb;

                        /* add samples */
                        nb = (samples_size - wanted_size);
                        samples_end = (uint8_t *)samples + samples_size - n;
                        q = samples_end + n;
                        while (nb > 0) {
                            memcpy(q, samples_end, n);
                            q += n;
                            nb -= n;
                        }
                        samples_size = wanted_size;
                    }
                }
#if 0
                printf("diff=%f adiff=%f sample_diff=%d apts=%0.3f vpts=%0.3f %f\n",
                       diff, avg_diff, samples_size - samples_size1,
                       is->audio_clock, is->video_clock, is->audio_diff_threshold);
#endif
            }
        } else {
            /* too big difference : may be initial PTS errors, so
               reset A-V filter */
            is->audio_diff_avg_count = 0;
            is->audio_diff_cum = 0;
        }
    }

    return samples_size;
}

/*!
 * Decode one audio frame into \p audio_buf of size \p buf_size and
 * returns its uncompressed size.
 */
static int audio_decode_frame(VideoState *is, uint8_t *audio_buf, int buf_size, double *pts_ptr)
{
    AVPacket *pkt = &is->audio_pkt;
    int n, len1, data_size;
    double pts;

    for(;;) {
        /* NOTE: the audio packet can contain several frames */
        while (is->audio_pkt_size > 0) {
            data_size = buf_size;
            len1 = avcodec_decode_audio2(is->audio_stream->codec,
                                         (int16_t *)audio_buf, &data_size,
                                         is->audio_pkt_data, is->audio_pkt_size);
            if (len1 < 0) {
                /* if error, we skip the frame */
                is->audio_pkt_size = 0;
                break;
            }

            is->audio_pkt_data += len1;
            is->audio_pkt_size -= len1;
            if (data_size <= 0)
                continue;
            /* if no pts, then compute it */
            pts = is->audio_clock;
            *pts_ptr = pts;
            n = 2 * is->audio_stream->codec->channels;
            is->audio_clock += (double)data_size /
                (double)(n * is->audio_stream->codec->sample_rate);
#if defined(DEBUG_SYNC)
            {
                static double last_clock;
                printf("audio: delay=%0.3f clock=%0.3f pts=%0.3f\n",
                       is->audio_clock - last_clock,
                       is->audio_clock, pts);
                last_clock = is->audio_clock;
            }
#endif
            return data_size;
        }

        /* free the current packet */
        if (pkt->data)
            av_free_packet(pkt);

        if (is->paused || is->audioq.abort_request) {
            return -1;
        }

        /* read next packet */
        if (packet_queue_get(&is->audioq, pkt, 1) < 0)
            return -1;
        if(pkt->data == g_flush_pkt.data) {
            avcodec_flush_buffers(is->audio_stream->codec);
            continue;
        }

        is->audio_pkt_data = pkt->data;
        is->audio_pkt_size = pkt->size;

        /* if update the audio clock with the pts */
        if (pkt->pts != AV_NOPTS_VALUE) {
            is->audio_clock = av_q2d(is->audio_stream->time_base)*pkt->pts;
        }
    }
}

/*!
 * Get the current audio output buffer size, in samples. With SDL,
 * we cannot have a precise information.
 */
static int audio_write_get_buf_size(VideoState *is)
{
    return is->audio_buf_size - is->audio_buf_index;
}

/*!
 * Prepare a new audio buffer.
 */
void sdl_audio_callback(void *opaque, uint8_t *stream, int len)
{
    VideoState *is = opaque;
    int audio_size, len1;
    double pts;

    audio_callback_time = av_gettime();

    while (len > 0) {
        if (is->audio_buf_index >= is->audio_buf_size) {
            audio_size = audio_decode_frame(is, is->audio_buf, sizeof(is->audio_buf), &pts);
            if (audio_size < 0) {
                /* if error, just output silence */
                is->audio_buf_size = 1024;
                memset(is->audio_buf, 0, is->audio_buf_size);
            } else {
                if (is->show_audio_flag)
                    update_sample_display(is, (int16_t *)is->audio_buf, audio_size);
                audio_size = synchronize_audio(is, (int16_t *)is->audio_buf, audio_size,
                                               pts);
                is->audio_buf_size = audio_size;
            }
            is->audio_buf_index = 0;
        }
        len1 = is->audio_buf_size - is->audio_buf_index;
        if (len1 > len)
            len1 = len;
        memcpy(stream, (uint8_t *)is->audio_buf + is->audio_buf_index, len1);
        len -= len1;
        stream += len1;
        is->audio_buf_index += len1;
    }
}

/*!
 * Open a given stream at \p is using stream index \p stream_index.
 * \return 0 if OK.
 */
static int stream_component_open(VideoState *is, int stream_index)
{
    AVFormatContext *ic = is->ic;
    AVCodecContext *enc;
    AVCodec *codec;
    SDL_AudioSpec wanted_spec, spec;

    if (stream_index < 0 || stream_index >= ic->nb_streams)
        return -1;
    enc = ic->streams[stream_index]->codec;

    /* prepare audio output */
    if (enc->codec_type == CODEC_TYPE_AUDIO) {
        wanted_spec.freq = enc->sample_rate;
        wanted_spec.format = AUDIO_S16SYS;
        /* hack for AC3. XXX: suppress that */
        if (enc->channels > 2) enc->channels = 2;
        wanted_spec.channels = enc->channels;
        wanted_spec.silence = 0;
        wanted_spec.samples = SDL_AUDIO_BUFFER_SIZE;
        wanted_spec.callback = sdl_audio_callback;
        wanted_spec.userdata = is;
        if (SDL_OpenAudio(&wanted_spec, &spec) < 0) {
            fprintf(stderr, "SDL_OpenAudio: %s\n", SDL_GetError());
            return -1;
        }
        is->audio_hw_buf_size = spec.size;
    }

    codec = avcodec_find_decoder(enc->codec_id);
    enc->debug_mv = debug_mv;
    enc->debug = debug;
    enc->workaround_bugs = workaround_bugs;
    enc->lowres = lowres;
    if(lowres) enc->flags |= CODEC_FLAG_EMU_EDGE;
    enc->idct_algo= idct;
    if(fast) enc->flags2 |= CODEC_FLAG2_FAST;
    enc->skip_frame= skip_frame;
    enc->skip_idct= skip_idct;
    enc->skip_loop_filter= skip_loop_filter;
    enc->error_resilience= error_resilience;
    enc->error_concealment= error_concealment;
    if (!codec ||
        avcodec_open(enc, codec) < 0)
        return -1;
    if(thread_count>1)
        avcodec_thread_init(enc, thread_count);
    enc->thread_count= thread_count;
    switch(enc->codec_type) {
    case CODEC_TYPE_AUDIO:
        is->audio_stream_index = stream_index;
        is->audio_stream = ic->streams[stream_index];
        is->audio_buf_size = 0;
        is->audio_buf_index = 0;

        /* init averaging filter */
        is->audio_diff_avg_coef = exp(log(0.01) / AUDIO_DIFF_AVG_NB);
        is->audio_diff_avg_count = 0;
        /* since we do not have a precise anough audio fifo fullness,
           we correct audio sync only if larger than this threshold */
        is->audio_diff_threshold = 2.0 * SDL_AUDIO_BUFFER_SIZE / enc->sample_rate;

        memset(&is->audio_pkt, 0, sizeof(is->audio_pkt));
        packet_queue_init(&is->audioq);
        SDL_PauseAudio(0);
        break;
    case CODEC_TYPE_VIDEO:
        is->video_stream_index = stream_index;
        is->video_stream = ic->streams[stream_index];

        is->frame_last_delay = 40e-3;
        is->frame_timer = (double)av_gettime() / 1000000.0;
        is->video_current_pts_time = av_gettime();

        packet_queue_init(&is->videoq);
        pthread_create(&is->video_tid, 0, video_thread, is);

        enc->get_buffer=     my_get_buffer;
        enc->release_buffer= my_release_buffer;
        break;
    case CODEC_TYPE_SUBTITLE:
        is->subtitle_stream_index = stream_index;
        is->subtitle_stream = ic->streams[stream_index];
        packet_queue_init(&is->subtitleq);

        pthread_create(&is->subtitle_tid, 0, subtitle_thread, is);
        break;
    default:
        break;
    }
    return 0;
}

static void stream_component_close(VideoState *is, int stream_index)
{
    AVFormatContext *ic = is->ic;
    AVCodecContext *enc;

    if (stream_index < 0 || stream_index >= ic->nb_streams)
        return;
    enc = ic->streams[stream_index]->codec;

    switch(enc->codec_type) {
    case CODEC_TYPE_AUDIO:
        packet_queue_abort(&is->audioq);

        SDL_CloseAudio();

        packet_queue_end(&is->audioq);
        break;
    case CODEC_TYPE_VIDEO:
        packet_queue_abort(&is->videoq);

        /* note: we also signal this mutex to make sure we deblock the
           video thread in all cases */
        pthread_mutex_lock(&is->pictQ_mutex);
        pthread_cond_signal(&is->pictQ_cond);
        pthread_mutex_unlock(&is->pictQ_mutex);

        pthread_join(is->video_tid, NULL);

        packet_queue_end(&is->videoq);
        break;
    case CODEC_TYPE_SUBTITLE:
        packet_queue_abort(&is->subtitleq);

        /* note: we also signal this mutex to make sure we deblock the
           video thread in all cases */
        pthread_mutex_lock(&is->subpQ_mutex);
        is->subtitle_stream_changed = 1;

        pthread_cond_signal(&is->subpQ_cond);
        pthread_mutex_unlock(&is->subpQ_mutex);

        pthread_join(is->subtitle_tid, NULL);

        packet_queue_end(&is->subtitleq);
        break;
    default:
        break;
    }

    avcodec_close(enc);
    switch(enc->codec_type) {
    case CODEC_TYPE_AUDIO:
        is->audio_stream = NULL;
        is->audio_stream_index = -1;
        break;
    case CODEC_TYPE_VIDEO:
        is->video_stream = NULL;
        is->video_stream_index = -1;
        break;
    case CODEC_TYPE_SUBTITLE:
        is->subtitle_stream = NULL;
        is->subtitle_stream_index = -1;
        break;
    default:
        break;
    }
}

static void dump_stream_info(const AVFormatContext *s)
{
    if (s->track != 0)
        fprintf(stderr, "Track: %d\n", s->track);
    if (s->title[0] != '\0')
        fprintf(stderr, "Title: %s\n", s->title);
    if (s->author[0] != '\0')
        fprintf(stderr, "Author: %s\n", s->author);
    if (s->copyright[0] != '\0')
        fprintf(stderr, "Copyright: %s\n", s->copyright);
    if (s->comment[0] != '\0')
        fprintf(stderr, "Comment: %s\n", s->comment);
    if (s->album[0] != '\0')
        fprintf(stderr, "Album: %s\n", s->album);
    if (s->year != 0)
        fprintf(stderr, "Year: %d\n", s->year);
    if (s->genre[0] != '\0')
        fprintf(stderr, "Genre: %s\n", s->genre);
}

/*!
 * Since we have only one decoding thread, we can use a global
 * variable instead of a thread local variable.
 */
static VideoState *global_video_state;

static int decode_interrupt_cb(void)
{
    return (global_video_state && global_video_state->abort_request);
}

/*!
 * This thread gets the stream from the disk or the network.
 */
static void * decode_thread(void *arg)
{
    VideoState *is = arg;
    AVFormatContext *ic;
    int err, ret, video_index, audio_index;
    uint i;
    AVPacket pkt1, *pkt = &pkt1;
    AVFormatParameters params, *ap = &params;

    video_index = -1;
    audio_index = -1;
    is->video_stream_index = -1;
    is->audio_stream_index = -1;
    is->subtitle_stream_index = -1;

    global_video_state = is;
    url_set_interrupt_cb(decode_interrupt_cb);

    memset(ap, 0, sizeof(*ap));

    ap->width = g_frame_width;
    ap->height= g_frame_height;
    ap->time_base= (AVRational) {1, 25};
    ap->pix_fmt = g_frame_pix_fmt;

    err = av_open_input_file(&ic, is->filename, is->iformat, 0, ap);
    if (err < 0) {
        fprintf(stderr, "%s: file:%s error:%d\n", __FUNCTION__, is->filename, err);
        ret = -1;
        goto fail;
    }
    is->ic = ic;

    if(genpts)
        ic->flags |= AVFMT_FLAG_GENPTS;

    err = av_find_stream_info(ic);
    if (err < 0) {
        fprintf(stderr, "%s: could not find codec parameters\n", is->filename);
        ret = -1;
        goto fail;
    }
    ic->pb.eof_reached= 0; //FIXME hack, ffplay maybe should not use url_feof() to test for the end

    /* if seeking requested, we execute it */
    if (g_start_time != AV_NOPTS_VALUE) {
        int64_t timestamp;

        timestamp = g_start_time;
        /* add the stream start time */
        if (ic->start_time != AV_NOPTS_VALUE)
            timestamp += ic->start_time;
        ret = av_seek_frame(ic, -1, timestamp, AVSEEK_FLAG_BACKWARD);
        if (ret < 0) {
            fprintf(stderr, "%s: could not seek to position %0.3f\n",
                    is->filename, (double)timestamp / AV_TIME_BASE);
        }
    }

    for(i = 0; i < ic->nb_streams; i++) {
        AVCodecContext *enc = ic->streams[i]->codec;
        switch(enc->codec_type) {
        case CODEC_TYPE_AUDIO:
            if ((audio_index < 0 || g_wanted_audio_stream-- > 0) && !g_audio_disable)
                audio_index = i;
            break;
        case CODEC_TYPE_VIDEO:
            if ((video_index < 0 || g_wanted_video_stream-- > 0) && !g_video_disable)
                video_index = i;
            break;
        default:
            break;
        }
    }
    if (g_show_status) {
        dump_format(ic, 0, is->filename, 0);
        dump_stream_info(ic);
    }

    /* open the streams */
    if (audio_index >= 0) {
        stream_component_open(is, audio_index);
    }

    if (video_index >= 0) {
        stream_component_open(is, video_index);
    } else {
        if (!g_display_disable)
            is->show_audio_flag = 1;
    }

    if (is->video_stream_index < 0 && is->audio_stream_index < 0) {
        fprintf(stderr, "%s: could not open codecs\n", is->filename);
        ret = -1;
        goto fail;
    }

    for(;;) {
        if (is->abort_request)
            break;
        if (is->paused != is->last_paused) {
            is->last_paused = is->paused;
            if (is->paused)
                av_read_pause(ic);
            else
                av_read_play(ic);
        }
#ifdef CONFIG_RTSP_DEMUXER
        if (is->paused && !strcmp(ic->iformat->name, "rtsp")) {
            /* wait 10 ms to avoid trying to get another packet */
            /* XXX: horrible */
            SDL_Delay(10);
            continue;
        }
#endif
        if (is->seek_req) {
            int stream_index= -1;
            int64_t seek_target= is->seek_pos;

            if     (is->video_stream_index >= 0) stream_index= is->video_stream_index;
            else if(is->audio_stream_index >= 0) stream_index= is->audio_stream_index;
            else if(is->subtitle_stream_index >= 0) stream_index= is->subtitle_stream_index;

            if(stream_index>=0) {
                seek_target= av_rescale_q(seek_target, AV_TIME_BASE_Q, ic->streams[stream_index]->time_base);
            }

            ret = av_seek_frame(is->ic, stream_index, seek_target, is->seek_flags);
            if (ret < 0) {
                fprintf(stderr, "%s: error while seeking\n", is->ic->filename);
            }else{
                if (is->audio_stream_index >= 0) {
                    packet_queue_flush(&is->audioq);
                    packet_queue_put(&is->audioq, &g_flush_pkt);
                }
                if (is->subtitle_stream_index >= 0) {
                    packet_queue_flush(&is->subtitleq);
                    packet_queue_put(&is->subtitleq, &g_flush_pkt);
                }
                if (is->video_stream_index >= 0) {
                    packet_queue_flush(&is->videoq);
                    packet_queue_put(&is->videoq, &g_flush_pkt);
                }
            }
            is->seek_req = 0;
        }

        /* if the queue are full, no need to read more */
        if (is->audioq.size > MAX_AUDIOQ_SIZE ||
            is->videoq.size > MAX_VIDEOQ_SIZE ||
            is->subtitleq.size > MAX_SUBTITLEQ_SIZE ||
            url_feof(&ic->pb)) {
            /* wait 10 ms */
            SDL_Delay(10);
            continue;
        }
        ret = av_read_frame(ic, pkt);
        if (ret < 0) {
            if (url_ferror(&ic->pb) == 0) {
                SDL_Delay(100); /* wait for user event */
                continue;
            } else
                break;
        }
        if (pkt->stream_index == is->audio_stream_index) {
            packet_queue_put(&is->audioq, pkt);
        } else if (pkt->stream_index == is->video_stream_index) {
            packet_queue_put(&is->videoq, pkt);
        } else if (pkt->stream_index == is->subtitle_stream_index) {
            packet_queue_put(&is->subtitleq, pkt);
        } else {
            av_free_packet(pkt);
        }
    }
    /* wait until the end */
    while (!is->abort_request) {
        SDL_Delay(100);
    }

    ret = 0;
fail:
    /* disable interrupting */
    global_video_state = NULL;

    /* close each stream */
    if (is->audio_stream_index >= 0)
        stream_component_close(is, is->audio_stream_index);
    if (is->video_stream_index >= 0)
        stream_component_close(is, is->video_stream_index);
    if (is->subtitle_stream_index >= 0)
        stream_component_close(is, is->subtitle_stream_index);
    if (is->ic) {
        av_close_input_file(is->ic);
        is->ic = NULL; /* safety */
    }
    url_set_interrupt_cb(NULL);

    if (ret != 0) {
        SDL_Event event;

        event.type = FF_QUIT_EVENT;
        event.user.data1 = is;
        SDL_PushEvent(&event);
    }
    return 0;
}

static VideoState *stream_open(const char *filename, AVInputFormat *iformat)
{
    VideoState *is = av_mallocz(sizeof(VideoState));
    if (!is) return NULL;

    av_strlcpy(is->filename, filename, sizeof(is->filename));
    is->iformat = iformat;
    is->ytop = 0;
    is->xleft = 0;
    is->img_convert_ctx = 0;
    is->last_time = 0;

    /* start video display */
    pthread_mutex_init(&is->pictQ_mutex, 0);
    pthread_cond_init(&is->pictQ_cond, NULL);

    pthread_mutex_init(&is->subpQ_mutex, 0);
    pthread_cond_init(&is->subpQ_cond, NULL);

    /* add the refresh timer to draw the picture */
    schedule_refresh(is, 40);

    is->av_sync_type = g_av_sync_type;
    pthread_create(&is->parse_tid, 0, decode_thread, is);
    if (!is->parse_tid) {
        av_free(is);
        return NULL;
    }
    return is;
}

static void stream_close(VideoState *is)
{
    VideoPicture *vp;
    int i;
    /* XXX: use a special url_shutdown call to abort parse cleanly */
    is->abort_request = 1;
    pthread_join(is->parse_tid, NULL);

    /* free all pictures */
    for(i=0;i<VIDEO_PICTURE_QUEUE_SIZE; i++) {
        vp = &is->pictQ[i];
        if (vp->bmp) {
            SDL_FreeYUVOverlay(vp->bmp);
            vp->bmp = NULL;
        }
    }
    pthread_mutex_destroy(&is->pictQ_mutex);
    pthread_cond_destroy(&is->pictQ_cond);
    pthread_mutex_destroy(&is->subpQ_mutex);
    pthread_cond_destroy(&is->subpQ_cond);
}

static void stream_cycle_channel(VideoState *is, int codec_type)
{
    AVFormatContext *ic = is->ic;
    int start_index, stream_index;
    AVStream *st;

    if (codec_type == CODEC_TYPE_VIDEO)
        start_index = is->video_stream_index;
    else if (codec_type == CODEC_TYPE_AUDIO)
        start_index = is->audio_stream_index;
    else
        start_index = is->subtitle_stream_index;
    if (start_index < (codec_type == CODEC_TYPE_SUBTITLE ? -1 : 0))
        return;
    stream_index = start_index;
    for(;;) {
        if (++stream_index >= is->ic->nb_streams)
        {
            if (codec_type == CODEC_TYPE_SUBTITLE)
            {
                stream_index = -1;
                goto the_end;
            } else
                stream_index = 0;
        }
        if (stream_index == start_index)
            return;
        st = ic->streams[stream_index];
        if (st->codec->codec_type == codec_type) {
            /* check that parameters are OK */
            switch(codec_type) {
            case CODEC_TYPE_AUDIO:
                if (st->codec->sample_rate != 0 &&
                    st->codec->channels != 0)
                    goto the_end;
                break;
            case CODEC_TYPE_VIDEO:
            case CODEC_TYPE_SUBTITLE:
                goto the_end;
            default:
                break;
            }
        }
    }
the_end:
    stream_component_close(is, start_index);
    stream_component_open(is, stream_index);
}


static void toggle_full_screen(void)
{
    is_full_screen = !is_full_screen;
    if (!g_fs_screen_width) {
        /* use default SDL method */
        //        SDL_WM_ToggleFullScreen(screen);
    }
    video_open(cur_stream);
}

static void toggle_pause(void)
{
    if (cur_stream)
        stream_pause(cur_stream);
    step = 0;
}

static void step_to_next_frame(void)
{
    if (cur_stream) {
        /* if the stream is paused unpause it, then step */
        if (cur_stream->paused)
            stream_pause(cur_stream);
    }
    step = 1;
}

static void do_exit(void)
{
    if (cur_stream) {
        stream_close(cur_stream);
        cur_stream = NULL;
    }
    if (g_show_status)
        printf("\n");
    SDL_Quit();
    exit(0);
}

static void toggle_audio_display(void)
{
    if (cur_stream) {
        cur_stream->show_audio_flag = !cur_stream->show_audio_flag;
    }
}

/*!
 * Handle an event sent by the GUI.
 */
static void event_loop(void)
{
    SDL_Event event;
    double incr, pos, frac;

    for(;;) {
        SDL_WaitEvent(&event);
        switch(event.type) {
        case SDL_KEYDOWN:
            switch(event.key.keysym.sym) {
            case SDLK_ESCAPE:
            case SDLK_q:
                do_exit();
                break;
            case SDLK_f:
                toggle_full_screen();
                break;
            case SDLK_p:
            case SDLK_SPACE:
                toggle_pause();
                break;
            case SDLK_s: //S: Step to next frame
                step_to_next_frame();
                break;
            case SDLK_a:
                if (cur_stream)
                    stream_cycle_channel(cur_stream, CODEC_TYPE_AUDIO);
                break;
            case SDLK_v:
                if (cur_stream)
                    stream_cycle_channel(cur_stream, CODEC_TYPE_VIDEO);
                break;
            case SDLK_t:
                if (cur_stream)
                    stream_cycle_channel(cur_stream, CODEC_TYPE_SUBTITLE);
                break;
            case SDLK_w:
                toggle_audio_display();
                break;
            case SDLK_LEFT:
                incr = -10.0;
                goto do_seek;
            case SDLK_RIGHT:
                incr = 10.0;
                goto do_seek;
            case SDLK_UP:
                incr = 60.0;
                goto do_seek;
            case SDLK_DOWN:
                incr = -60.0;
            do_seek:
                if (cur_stream) {
                    if (g_seek_by_bytes) {
                        pos = url_ftell(&cur_stream->ic->pb);
                        if (cur_stream->ic->bit_rate)
                            incr *= cur_stream->ic->bit_rate / 60.0;
                        else
                            incr *= 180000.0;
                        pos += incr;
                        stream_seek(cur_stream, pos, incr);
                    } else {
                        pos = get_master_clock(cur_stream);
                        pos += incr;
                        stream_seek(cur_stream, (int64_t)(pos * AV_TIME_BASE), incr);
                    }
                }
                break;
            default:
                break;
            }
            break;
        case SDL_MOUSEBUTTONDOWN:
            if (cur_stream) {
                int ns, hh, mm, ss;
                int tns, thh, tmm, tss;
                tns = cur_stream->ic->duration/1000000LL;
                thh = tns/3600;
                tmm = (tns%3600)/60;
                tss = (tns%60);
                frac = (double)event.button.x/(double)cur_stream->width;
                ns = frac*tns;
                hh = ns/3600;
                mm = (ns%3600)/60;
                ss = (ns%60);
                fprintf(stderr, "Seek to %2.0f%% (%2d:%02d:%02d) of total duration (%2d:%02d:%02d)       \n", frac*100,
                        hh, mm, ss, thh, tmm, tss);
                stream_seek(cur_stream, (int64_t)(cur_stream->ic->start_time+frac*cur_stream->ic->duration), 0);
            }
            break;
        case SDL_VIDEORESIZE:
            if (cur_stream) {
                screen = SDL_SetVideoMode(event.resize.w, event.resize.h, 0,
                                          SDL_HWSURFACE|SDL_RESIZABLE|SDL_ASYNCBLIT|SDL_HWACCEL);
                g_screen_width = cur_stream->width = event.resize.w;
                g_screen_height= cur_stream->height= event.resize.h;
            }
            break;
        case SDL_QUIT:
        case FF_QUIT_EVENT:
            do_exit();
            break;
        case FF_ALLOC_EVENT:
            video_open(event.user.data1);
            alloc_picture(event.user.data1);
            break;
        case FF_REFRESH_EVENT:
            video_refresh_timer(event.user.data1);
            break;
        default:
            break;
        }
    }
}

static void opt_frame_size(const char *arg)
{
    if (av_parse_video_frame_size(&g_frame_width, &g_frame_height, arg) < 0) {
        fprintf(stderr, "Incorrect frame size\n");
        exit(1);
    }
    if ((g_frame_width % 2) != 0 || (g_frame_height % 2) != 0) {
        fprintf(stderr, "Frame size must be a multiple of 2\n");
        exit(1);
    }
}

static void opt_width(const char *arg)
{
    g_screen_width = atoi(arg);
    if(g_screen_width<=0) {
        fprintf(stderr, "invalid width\n");
        exit(1);
    }
}

static void opt_height(const char *arg)
{
    g_screen_height = atoi(arg);
    if(g_screen_height<=0) {
        fprintf(stderr, "invalid height\n");
        exit(1);
    }
}

static void opt_format(const char *arg)
{
    g_file_iformat = av_find_input_format(arg);
    if (!g_file_iformat) {
        fprintf(stderr, "Unknown input format: %s\n", arg);
        exit(1);
    }
}

static void opt_frame_pix_fmt(const char *arg)
{
    g_frame_pix_fmt = avcodec_get_pix_fmt(arg);
}

#ifdef CONFIG_RTSP_DEMUXER
static void opt_rtp_tcp(void)
{
    /* only tcp protocol */
    rtsp_default_protocols = (1 << RTSP_PROTOCOL_RTP_TCP);
}
#endif

static void opt_sync(const char *arg)
{
    if (!strcmp(arg, "audio"))
        g_av_sync_type = AV_SYNC_AUDIO_MASTER;
    else if (!strcmp(arg, "video"))
        g_av_sync_type = AV_SYNC_VIDEO_MASTER;
    else if (!strcmp(arg, "ext"))
        g_av_sync_type = AV_SYNC_EXTERNAL_CLOCK;
    else {
        show_help();
        exit(1);
    }
}

static void opt_seek(const char *arg)
{
    g_start_time = parse_date(arg, 1);
    if (g_start_time == INT64_MIN) {
        fprintf(stderr, "Invalid duration specification: %s\n", arg);
        exit(1);
    }
}

static void opt_debug(const char *arg)
{
    av_log_level = 99;
    debug = atoi(arg);
}

static void opt_vismv(const char *arg)
{
    debug_mv = atoi(arg);
}

static void opt_thread_count(const char *arg)
{
    thread_count= atoi(arg);
#if !defined(HAVE_THREADS)
    fprintf(stderr, "Warning: not compiled with thread support, using thread emulation\n");
#endif
}

static void opt_show_help(void)
{
    show_help();
    exit(0);
}

const OptionDef options[] = {
    { "h", 0, {(void*)opt_show_help}, "show help" },
    { "x", HAS_ARG, {(void*)opt_width}, "force displayed width", "width" },
    { "y", HAS_ARG, {(void*)opt_height}, "force displayed height", "height" },
    { "s", HAS_ARG | OPT_VIDEO, {(void*)opt_frame_size}, "set frame size (WxH or abbreviation)", "size" },
    { "fs", OPT_BOOL, {(void*)&is_full_screen}, "force full screen" },
    { "an", OPT_BOOL, {(void*)&g_audio_disable}, "disable audio" },
    { "vn", OPT_BOOL, {(void*)&g_video_disable}, "disable video" },
    { "ast", OPT_INT | HAS_ARG | OPT_EXPERT, {(void*)&g_wanted_audio_stream}, "", "" },
    { "vst", OPT_INT | HAS_ARG | OPT_EXPERT, {(void*)&g_wanted_video_stream}, "", "" },
    { "ss", HAS_ARG, {(void*)&opt_seek}, "seek to a given position in seconds", "pos" },
    { "bytes", OPT_BOOL, {(void*)&g_seek_by_bytes}, "seek by bytes" },
    { "nodisp", OPT_BOOL, {(void*)&g_display_disable}, "disable graphical display" },
    { "f", HAS_ARG, {(void*)opt_format}, "force format", "fmt" },
    { "pix_fmt", HAS_ARG | OPT_EXPERT | OPT_VIDEO, {(void*)opt_frame_pix_fmt}, "set pixel format", "format" },
    { "stats", OPT_BOOL | OPT_EXPERT, {(void*)&g_show_status}, "show status", "" },
    { "debug", HAS_ARG | OPT_EXPERT, {(void*)opt_debug}, "print specific debug info", "" },
    { "bug", OPT_INT | HAS_ARG | OPT_EXPERT, {(void*)&workaround_bugs}, "workaround bugs", "" },
    { "vismv", HAS_ARG | OPT_EXPERT, {(void*)opt_vismv}, "visualize motion vectors", "" },
    { "fast", OPT_BOOL | OPT_EXPERT, {(void*)&fast}, "non spec compliant optimizations", "" },
    { "genpts", OPT_BOOL | OPT_EXPERT, {(void*)&genpts}, "generate pts", "" },
    { "drp", OPT_BOOL |OPT_EXPERT, {(void*)&decoder_reorder_pts}, "let decoder reorder pts", ""},
    { "lowres", OPT_INT | HAS_ARG | OPT_EXPERT, {(void*)&lowres}, "", "" },
    { "skiploop", OPT_INT | HAS_ARG | OPT_EXPERT, {(void*)&skip_loop_filter}, "", "" },
    { "skipframe", OPT_INT | HAS_ARG | OPT_EXPERT, {(void*)&skip_frame}, "", "" },
    { "skipidct", OPT_INT | HAS_ARG | OPT_EXPERT, {(void*)&skip_idct}, "", "" },
    { "idct", OPT_INT | HAS_ARG | OPT_EXPERT, {(void*)&idct}, "set idct algo",  "algo" },
    { "er", OPT_INT | HAS_ARG | OPT_EXPERT, {(void*)&error_resilience}, "set error detection threshold (0-4)",  "threshold" },
    { "ec", OPT_INT | HAS_ARG | OPT_EXPERT, {(void*)&error_concealment}, "set error concealment options",  "bit_mask" },
#ifdef CONFIG_RTSP_DEMUXER
    { "rtp_tcp", OPT_EXPERT, {(void*)&opt_rtp_tcp}, "force RTP/TCP protocol usage", "" },
#endif
    { "sync", HAS_ARG | OPT_EXPERT, {(void*)opt_sync}, "set audio-video sync. type (type=audio/video/ext)", "type" },
    { "threads", HAS_ARG | OPT_EXPERT, {(void*)opt_thread_count}, "thread count", "count" },
    { NULL, },
};

void show_help(void)
{
    printf("usage: ffplay [options] input_file\n"
           "Simple media player\n");
    printf("\n");
    show_help_options(options, "Main options:\n",
                      OPT_EXPERT, 0);
    show_help_options(options, "\nAdvanced options:\n",
                      OPT_EXPERT, OPT_EXPERT);
    printf("\nWhile playing:\n"
           "q, ESC              quit\n"
           "f                   toggle full screen\n"
           "p, SPC              pause\n"
           "a                   cycle audio channel\n"
           "v                   cycle video channel\n"
           "t                   cycle subtitle channel\n"
           "w                   show audio waves\n"
           "left/right          seek backward/forward 10 seconds\n"
           "down/up             seek backward/forward 1 minute\n"
           "mouse click         seek to percentage in file corresponding to fraction of width\n"
           );
}

void opt_input_file(const char *filename)
{
    if (!strcmp(filename, "-"))
        filename = "pipe:";
    g_input_filename = filename;
}

#endif

int av_ffmpeg_playback_test(int argc, char **argv)
{
#if 0
    int flags;

    av_register_all();	/* register all codecs, demux and protocols */

    parse_options(argc, argv, options, opt_input_file);

    if (!g_input_filename) { show_help(); exit(1); }

    if (g_display_disable) { g_video_disable = 1; }
    flags = SDL_INIT_VIDEO | SDL_INIT_AUDIO | SDL_INIT_TIMER;
#if !defined(__MINGW32__) && !defined(__APPLE__)
    flags |= SDL_INIT_EVENTTHREAD; /* Not supported on Windows or Mac OS X */
#endif
    if (SDL_Init (flags)) {
        fprintf(stderr, "Could not initialize SDL - %s\n", SDL_GetError());
        exit(1);
    }

    if (!g_display_disable) {
#ifdef HAVE_SDL_VIDEO_SIZE
        const SDL_VideoInfo *vi = SDL_GetVideoInfo();
        g_fs_screen_width = vi->current_w;
        g_fs_screen_height = vi->current_h;
#endif
    }

    SDL_EventState(SDL_ACTIVEEVENT, SDL_IGNORE);
    SDL_EventState(SDL_MOUSEMOTION, SDL_IGNORE);
    SDL_EventState(SDL_SYSWMEVENT, SDL_IGNORE);
    SDL_EventState(SDL_USEREVENT, SDL_IGNORE);

    av_init_packet(&g_flush_pkt);
    g_flush_pkt.data = "FLUSH";

    cur_stream = stream_open(g_input_filename, g_file_iformat);

    event_loop();
#endif
    return 0;
}
