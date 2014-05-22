/*!
 * \file lamexx.hpp
 * \brief C++ Wrapper for LAME.
 * \author Copyright (C) 2012 Per Nordlöw (per.nordlow@gmail.com)
 *
 *  Re: Thread safe LAME?: msg#00076
 *  Subject: 	Re: Thread safe LAME?
 *
 *  All static variables used by the encoder are
 *  initialized during beInitStream() and they're always
 *  initialized to the same values, so you can use LAME in
 *  a multi-threaded application provided you follow the
 *  following rules:
 *
 *  1) Link LAME with the multi-threaded version of the C
 *  runtime library.
 *
 *  2) Don't use the decoder.
 *
 *  3) serialize all calls to beInitStream() with all
 *  other calls to the LAME API over all hbeStreams.
 *
 *  4) serialize all calls to the other API per each
 *  hbeStream your program creates.
 *
 *  number 3) is a little bit difficult to achieve.  You
 *  can do it by a single-writer/multiple-readers type of
 *  lock.  When calling beInitStream() grab the lock as a
 *  writer.  When calling all other API grab the lock as a
 *  reader.  A simpler way to do it would be to structure
 *  your application accordingly, for instance, to create
 *  all the hbeStreams you want sequentially at the start
 *  of your application, and after that just use the
 *  streams, don't make new ones.
 *
 *  number 4) can be achieved by using one mutex per
 *  hbeStream.  This will allow you to encode multiple
 *  streams concurrently from different threads.
 *
 *  Beware that LAME is a CPU-intensive process, so unless
 *  you're running on a computer with multiple CPUs, there
 *  really is no point in encoding multiple streams
 *  simultaneously.  They'll just slow each other down.
 *  Even Intel's hyper-threading won't be much help,
 *  because they only have one execution engine in the
 *  processor.
 *
 *
 *  Regards,
 *  Ofer Porat
 */

#pragma once
#include <lame/lame.h>

namespace lame {
    class Encoder;
}

class lame::Encoder {
public:
    Encoder() {
	m_gflags = lame_init(); // sets default for all encoder parameters

	/* required call to initialize decoder NOTE: the decoder
	 * should not be used when encoding is performed with
	 * decoding on the fly */
	lame_decode_init();
    }

    /********************************************************************
     *  input stream description
     ***********************************************************************/
    /*! number of samples.  default = 2^32-1   */
    int set_num_samples(unsigned long n) { return ::lame_set_num_samples(m_gflags, n); }
    unsigned long get_num_samples(const lame_global_flags *) { return ::lame_get_num_samples(m_gflags); }

    /*! input sample rate in Hz.  default = 44100hz */
    int set_in_samplerate(int srate) { return ::lame_set_in_samplerate(m_gflags, srate); }
    int get_in_samplerate() const { return ::lame_get_in_samplerate(m_gflags); }

    /*! number of channels in input stream. default=2  */
    int set_num_channels(int channels) { return ::lame_set_scale(m_gflags, channels); }
    int get_num_channels() const { return ::lame_get_num_channels(m_gflags); };

    /*
      scale the input by this amount before encoding.  default=0 (disabled)
      (not used by decoding routines)
    */
    int set_scale(float scale) { return ::lame_set_scale(m_gflags, scale); }
    float get_scale() const { return ::lame_get_scale(m_gflags); };

    /*
      scale the channel 0 (left) input by this amount before encoding.
      default=0 (disabled)
      (not used by decoding routines)
    */
    int set_scale_left(float scale) { return ::lame_set_scale_left(m_gflags, scale); }
    float get_scale_left() const { return ::lame_get_scale_left(m_gflags); }

    /*
      scale the channel 1 (right) input by this amount before encoding.
      default=0 (disabled)
      (not used by decoding routines)
    */
    int set_scale_right(float scale) { return ::lame_set_scale_right(m_gflags, scale); }
    float get_scale_right() const { return ::lame_get_scale_right(m_gflags); }

    /*
      output sample rate in Hz.  default = 0, which means LAME picks best value
      based on the amount of compression.  MPEG only allows:
      MPEG1    32, 44.1,   48khz
      MPEG2    16, 22.05,  24
      MPEG2.5   8, 11.025, 12
      (not used by decoding routines)
    */
    int set_out_samplerate(int srate) { return ::lame_set_out_samplerate(m_gflags, srate); }
    int get_out_samplerate() const { return ::lame_get_out_samplerate(m_gflags); }


    /********************************************************************
     *  general control parameters
     ***********************************************************************/
    /* 1=cause LAME to collect data for an MP3 frame analyzer. default=0 */
    int set_analysis(int x) { return ::lame_set_analysis(m_gflags, x); };
    int get_analysis() const { return ::lame_get_analysis(m_gflags); }

    /*
      1 = write a Xing VBR header frame.
      default = 1
      this variable must have been added by a Hungarian notation Windows programmer :-)
    */
    int set_bWriteVbrTag(int x) { return ::lame_set_bWriteVbrTag(m_gflags, x); };
    int get_bWriteVbrTag() const { return ::lame_get_bWriteVbrTag(m_gflags); }

    /* 1=decode only.  use lame/mpglib to convert mp3/ogg to wav.  default=0 */
    int set_decode_only(int x) { return ::lame_set_decode_only(m_gflags, x); }
    int get_decode_only() const { return ::lame_get_decode_only(m_gflags); }

    /*
      internal algorithm selection.  True quality is determined by the bitrate
      but this variable will effect quality by selecting expensive or cheap algorithms.
      quality=0..9.  0=best (very slow).  9=worst.
      recommended:  2     near-best quality, not too slow
      5     good quality, fast
      7     ok quality, really fast
    */
    int set_quality(int x) { return ::lame_set_quality(m_gflags, x); };
    int get_quality() const { return ::lame_get_quality(m_gflags); }

    /*
      mode = 0,1,2,3 = stereo, jstereo, dual channel (not supported), mono
      default: lame picks based on compression ration and input channels
    */
    int set_mode(MPEG_mode x) { return ::lame_set_mode(m_gflags, x); }
    MPEG_mode get_mode() const { return ::lame_get_mode(m_gflags); }

    /*
      force_ms.  Force M/S for all frames.  For testing only.
      default = 0 (disabled)
    */
    int set_force_ms(int x) { return ::lame_set_force_ms(m_gflags, x); }
    int get_force_ms() const { return ::lame_get_force_ms(m_gflags); }

    /* use free_format?  default = 0 (disabled) */
    int set_free_format(int x) { return ::lame_set_free_format(m_gflags, x); }
    int get_free_format() const { return ::lame_get_free_format(m_gflags); }

    /* perform ReplayGain analysis?  default = 0 (disabled) */
    int set_findReplayGain(int x) { return ::lame_set_findReplayGain(m_gflags, x); };
    int get_findReplayGain() const { return ::lame_get_findReplayGain(m_gflags); }

    /* decode on the fly. Search for the peak sample. If the ReplayGain
     * analysis is enabled then perform the analysis on the decoded data
     * stream. default = 0 (disabled)
     * NOTE: if this option is set the build-in decoder should not be used */
    int set_decode_on_the_fly(int x) { return ::lame_set_decode_on_the_fly(m_gflags, x); }
    int get_decode_on_the_fly() const { return ::lame_get_decode_on_the_fly(m_gflags); }

    /* counters for gapless encoding */
    int set_nogap_total(lame_global_flags*, int);
    int get_nogap_total() const { return ::lame_get_nogap_total(m_gflags); }

    int set_nogap_currentindex(lame_global_flags* , int);
    int get_nogap_currentindex() const { return ::lame_get_nogap_currentindex(m_gflags); }


    /*
     * OPTIONAL:
     * Set printf like error/debug/message reporting functions.
     * The second argument has to be a pointer to a function which looks like
     *   void my_debugf(const char *format, va_list ap)
     *   {
     *       (void) vfprintf(stdout, format, ap);
     *   }
     * If you use NULL as the value of the pointer in the set function, the
     * lame buildin function will be used (prints to stderr).
     * To quiet any output you have to replace the body of the example function
     * with just "return;" and use it in the set function.
     */
    int set_errorf(lame_global_flags *,
		   void (*func)(const char *, va_list));
    int set_debugf(lame_global_flags *,
		   void (*func)(const char *, va_list));
    int set_msgf  (lame_global_flags *,
		   void (*func)(const char *, va_list));



    /* set one of brate compression ratio.  default is compression ratio of 11.  */
    int set_brate(int x) { return ::lame_set_brate(m_gflags, x); };
    int get_brate() const { return ::lame_get_brate(m_gflags); }
    int set_compression_ratio(float x) { return ::lame_set_compression_ratio(m_gflags, x); }
    float get_compression_ratio() const { return ::lame_get_compression_ratio(m_gflags); }


    int set_preset( lame_global_flags*  gfp, int );
    int set_asm_optimizations( lame_global_flags*  gfp, int, int );



    /********************************************************************
     *  frame params
     ***********************************************************************/
    /* mark as copyright.  default=0 */
    int set_copyright(int x) { return ::lame_set_copyright(m_gflags, x); };
    int get_copyright() const { return ::lame_get_copyright(m_gflags); }

    /* mark as original.  default=1 */
    int set_original(int x) { return ::lame_set_original(m_gflags, x); };
    int get_original() const { return ::lame_get_original(m_gflags); }

    /* error_protection.  Use 2 bytes from each frame for CRC checksum. default=0 */
    int set_error_protection(int x) { return ::lame_set_error_protection(m_gflags, x); }
    int get_error_protection() const { return ::lame_get_error_protection(m_gflags); }

    /* MP3 'private extension' bit  Meaningless.  default=0 */
    int set_extension(int x) { return ::lame_set_extension(m_gflags, x); };
    int get_extension() const { return ::lame_get_extension(m_gflags); }

    /* enforce strict ISO compliance.  default=0 */
    int set_strict_ISO(int x) { return ::lame_set_strict_ISO(m_gflags, x); }
    int get_strict_ISO() const { return ::lame_get_strict_ISO(m_gflags); }


    /********************************************************************
     * quantization/noise shaping
     ***********************************************************************/

    /* disable the bit reservoir. For testing only. default=0 */
    int set_disable_reservoir(int x) { return ::lame_set_disable_reservoir(m_gflags, x); }
    int get_disable_reservoir() const { return ::lame_get_disable_reservoir(m_gflags); }

    /* select a different "best quantization" function. default=0  */
    int set_quant_comp(int x) { return ::lame_set_quant_comp(m_gflags, x); }
    int get_quant_comp() const { return ::lame_get_quant_comp(m_gflags); }
    int set_quant_comp_short(int x) { return ::lame_set_quant_comp_short(m_gflags, x); }
    int get_quant_comp_short() const { return ::lame_get_quant_comp_short(m_gflags); }

    int set_experimentalX(int x) { return ::lame_set_experimentalX(m_gflags, x); }; /* compatibility*/
    int get_experimentalX() const { return ::lame_get_experimentalX(m_gflags); }

    /* another experimental option.  for testing only */
    int set_experimentalY(int x) { return ::lame_set_experimentalY(m_gflags, x); };
    int get_experimentalY() const { return ::lame_get_experimentalY(m_gflags); }

    /* another experimental option.  for testing only */
    int set_experimentalZ(int x) { return ::lame_set_experimentalZ(m_gflags, x); };
    int get_experimentalZ() const { return ::lame_get_experimentalZ(m_gflags); }

    /* Naoki's psycho acoustic model.  default=0 */
    int set_exp_nspsytune(int x) { return ::lame_set_exp_nspsytune(m_gflags, x); }
    int get_exp_nspsytune() const { return ::lame_get_exp_nspsytune(m_gflags); }

    void set_msfix(double x) { return ::lame_set_msfix(m_gflags, x); }
    float get_msfix() const { return ::lame_get_msfix(m_gflags); }

    int set_exp_nspsytune2_int( lame_global_flags*, int, int);
    float set_exp_nspsytune2_real( lame_global_flags*, int, float);
    void * set_exp_nspsytune2_pointer( lame_global_flags*, int, void *);

    /********************************************************************
     * VBR control
     ***********************************************************************/
    /* Types of VBR.  default = vbr_off = CBR */
    int set_VBR(vbr_mode x) { return ::lame_set_VBR(m_gflags, x); }
    vbr_mode get_VBR() const { return ::lame_get_VBR(m_gflags); }

    /* VBR quality level.  0=highest  9=lowest  */
    int set_VBR_q(int x) { return ::lame_set_VBR_q(m_gflags, x); }
    int get_VBR_q() const { return ::lame_get_VBR_q(m_gflags); }

    /* Ignored except for VBR=vbr_abr (ABR mode) */
    int set_VBR_mean_bitrate_kbps(int x) { return ::lame_set_VBR_mean_bitrate_kbps(m_gflags, x); }
    int get_VBR_mean_bitrate_kbps() const { return ::lame_get_VBR_mean_bitrate_kbps(m_gflags); }

    int set_VBR_min_bitrate_kbps(int x) { return ::lame_set_VBR_min_bitrate_kbps(m_gflags, x); }
    int get_VBR_min_bitrate_kbps() const { return ::lame_get_VBR_min_bitrate_kbps(m_gflags); }

    int set_VBR_max_bitrate_kbps(int x) { return ::lame_set_VBR_max_bitrate_kbps(m_gflags, x); }
    int get_VBR_max_bitrate_kbps() const { return ::lame_get_VBR_max_bitrate_kbps(m_gflags); }

    /*
      1=strictly enforce VBR_min_bitrate.  Normally it will be violated for
      analog silence
    */
    int set_VBR_hard_min(int x) { return ::lame_set_VBR_hard_min(m_gflags, x); }
    int get_VBR_hard_min() const { return ::lame_get_VBR_hard_min(m_gflags); }


    /********************************************************************
     * Filtering control
     ***********************************************************************/
    /* freq in Hz to apply lowpass. Default = 0 = lame chooses.  -1 = disabled */
    int set_lowpassfreq(int x) { return ::lame_set_lowpassfreq(m_gflags, x); };
    int get_lowpassfreq() const { return ::lame_get_lowpassfreq(m_gflags); }
    /* width of transition band, in Hz.  Default = one polyphase filter band */
    int set_lowpasswidth(int x) { return ::lame_set_lowpasswidth(m_gflags, x); };
    int get_lowpasswidth() const { return ::lame_get_lowpasswidth(m_gflags); }

    /* freq in Hz to apply highpass. Default = 0 = lame chooses.  -1 = disabled */
    int set_highpassfreq(int x) { return ::lame_set_highpassfreq(m_gflags, x); };
    int get_highpassfreq() const { return ::lame_get_highpassfreq(m_gflags); }
    /* width of transition band, in Hz.  Default = one polyphase filter band */
    int set_highpasswidth(int x) { return ::lame_set_highpasswidth(m_gflags, x); };
    int get_highpasswidth() const { return ::lame_get_highpasswidth(m_gflags); }


    /********************************************************************
     * psycho acoustics and other arguments which you should not change
     * unless you know what you are doing
     ***********************************************************************/

    /* only use ATH for masking */
    int set_ATHonly(int x) { return ::lame_set_ATHonly(m_gflags, x); };
    int get_ATHonly() const { return ::lame_get_ATHonly(m_gflags); }

    /* only use ATH for short blocks */
    int set_ATHshort(int x) { return ::lame_set_ATHshort(m_gflags, x); };
    int get_ATHshort() const { return ::lame_get_ATHshort(m_gflags); }

    /* disable ATH */
    int set_noATH(int x) { return ::lame_set_noATH(m_gflags, x); };
    int get_noATH() const { return ::lame_get_noATH(m_gflags); }

    /* select ATH formula */
    int set_ATHtype(int x) { return ::lame_set_ATHtype(m_gflags, x); };
    int get_ATHtype() const { return ::lame_get_ATHtype(m_gflags); }

    /* lower ATH by this many db */
    int set_ATHlower(float x) { return ::lame_set_ATHlower(m_gflags, x); }
    float get_ATHlower() const { return ::lame_get_ATHlower(m_gflags); }

    /* select ATH adaptive adjustment type */
    int set_athaa_type( lame_global_flags *, int);
    int get_athaa_type( ) const;

    /* select the loudness approximation used by the ATH adaptive auto-leveling  */
    int set_athaa_loudapprox( lame_global_flags *, int);
    int get_athaa_loudapprox( ) const;

    /* adjust (in dB) the point below which adaptive ATH level adjustment occurs */
    int set_athaa_sensitivity( lame_global_flags *, float);
    float get_athaa_sensitivity() const { return ::lame_get_athaa_sensitivity(m_gflags); }

    /*
      allow blocktypes to differ between channels?
      default: 0 for jstereo, 1 for stereo
    */
    int set_allow_diff_short(int x) { return ::lame_set_allow_diff_short(m_gflags, x); }
    int get_allow_diff_short() const { return ::lame_get_allow_diff_short(m_gflags); }

    /* use temporal masking effect (default = 1) */
    int set_useTemporal(int x) { return ::lame_set_useTemporal(m_gflags, x); };
    int get_useTemporal() const { return ::lame_get_useTemporal(m_gflags); }

    /* use temporal masking effect (default = 1) */
    int set_interChRatio(float x) { return ::lame_set_interChRatio(m_gflags, x); }
    float get_interChRatio() const { return ::lame_get_interChRatio(m_gflags); }

    /* disable short blocks */
    int set_no_short_blocks(int x) { return ::lame_set_no_short_blocks(m_gflags, x); }
    int get_no_short_blocks() const { return ::lame_get_no_short_blocks(m_gflags); }

    /* force short blocks */
    int set_force_short_blocks(int x) { return ::lame_set_force_short_blocks(m_gflags, x); }
    int get_force_short_blocks() const { return ::lame_get_force_short_blocks(m_gflags); }

    /* Input PCM is emphased PCM (for instance from one of the rarely
       emphased CDs), it is STRONGLY not recommended to use this, because
       psycho does not take it into account, and last but not least many decoders
       ignore these bits */
    int set_emphasis(int x) { return ::lame_set_emphasis(m_gflags, x); };
    int get_emphasis() const { return ::lame_get_emphasis(m_gflags); }



    /************************************************************************/
    /* internal variables, cannot be set...                                 */
    /* provided because they may be of use to calling application           */
    /************************************************************************/
    /* version  0=MPEG-2  1=MPEG-1  (2=MPEG-2.5)     */
    int get_version() const { return ::lame_get_version(m_gflags); }

    /* encoder delay   */
    int get_encoder_delay() const { return ::lame_get_encoder_delay(m_gflags); }

    /*
      padding appended to the input to make sure decoder can fully decode
      all input.  Note that this value can only be calculated during the
      call to lame_encoder_flush().  Before lame_encoder_flush() has
      been called, the value of encoder_padding = 0.
    */
    int get_encoder_padding() const { return ::lame_get_encoder_padding(m_gflags); }

    /* size of MPEG frame */
    int get_framesize() const { return ::lame_get_framesize(m_gflags); }

    /* number of PCM samples buffered, but not yet encoded to mp3 data. */
    int get_mf_samples_to_encode() const { return ::lame_get_mf_samples_to_encode(m_gflags); }

    /*
      size (bytes) of mp3 data buffered, but not yet encoded.
      this is the number of bytes which would be output by a call to
      lame_encode_flush_nogap.  NOTE: lame_encode_flush() will return
      more bytes than this because it will encode the reamining buffered
      PCM samples before flushing the mp3 buffers.
    */
    int get_size_mp3buffer() const { return ::lame_get_size_mp3buffer(m_gflags); }

    /* number of frames encoded so far */
    int get_frameNum() const { return ::lame_get_frameNum(m_gflags); }

    /*
      lame's estimate of the total number of frames to be encoded
      only valid if calling program set num_samples
    */
    int get_totalframes() const { return ::lame_get_totalframes(m_gflags); }

    /* RadioGain value. Multiplied by 10 and rounded to the nearest. */
    int get_RadioGain() const { return ::lame_get_RadioGain(m_gflags); }

    /* AudiophileGain value. Multipled by 10 and rounded to the nearest. */
    int get_AudiophileGain() const { return ::lame_get_AudiophileGain(m_gflags); }

    /* the peak sample */
    float get_PeakSample() const { return ::lame_get_PeakSample(m_gflags); }

    /* Gain change required for preventing clipping. The value is correct only if
       peak sample searching was enabled. If negative then the waveform
       already does not clip. The value is multiplied by 10 and rounded up. */
    int get_noclipGainChange() const { return ::lame_get_noclipGainChange(m_gflags); }

    /* user-specified scale factor required for preventing clipping. Value is
       correct only if peak sample searching was enabled and no user-specified
       scaling was performed. If negative then either the waveform already does
       not clip or the value cannot be determined */
    float get_noclipScale() const { return ::lame_get_noclipScale(m_gflags); }

    /*
     * REQUIRED:
     * sets more internal configuration based on data provided above.
     * returns -1 if something failed.
     */
    int lame_init_params(lame_global_flags * const );

    /*
     * OPTIONAL:
     * get the version number, in a string. of the form:
     * "3.63 (beta)" or just "3.63".
     */
    const char*  get_version       ( void );
    const char*  get_short_version ( void );
    const char*  get_very_short_version ( void );
    const char*  get_psy_version        ( void );
    const char*  get_lame_url           ( void );
protected:
private:
    lame_global_flags *m_gflags;
};
