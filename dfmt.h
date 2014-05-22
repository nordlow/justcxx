/*! \file fkind.h
 * \brief Data Kind (Format).
 * \author Copyright (C) 2012 Per Nordlöw (per.nordlow@gmail.com)
 * \see http://en.wikipedia.org/wiki/Alphabetical_list_of_file_extensions
 */

#pragma once

#include <unistd.h>
#include <stdbool.h>
#ifdef HAVE_CONFIG_H
#include "config.h"
#endif
#include "xstr.h"

#ifdef __cplusplus
extern "C" {
#endif

/* ========================================================================= */

/*! Data Pattern (File Contents) Type Code.
 *
 * Interpreted as either:
 *
 * - File \em Type / \em Kind / \em Formatt
 * - (Network) \em Protocol Format
 * - Computer Programming \em Language
 */
typedef enum {
  DFMT_any_,                 /**< Any of the following. */

  /** \name Binary Data. */
  /* @{ */
  DFMT_BINARY_any_,
  /* @} */

  /** \name Executable files. */
  /* @{ */
  DFMT_ELF,                     /**< Executable and Linkable Format (ELF) */
  DFMT_ELF_OBJ,                 /**< ELF Executable */
  DFMT_ELF_EXE,                 /**< ELF Object */
  /* @} */

  DFMT_LIBTOOL_LIBRARY_FILE,        /**< libtool library file */

  /** \name Audio */
  /* @{ */
  DFMT_AUDIO_any_,                        /**< Audio. */
  DFMT_AUDIO_MP3,
  DFMT_AUDIO_WAV,
  DFMT_AUDIO_AMR,
  DFMT_AUDIO_MPEG4,
  DFMT_AUDIO_last = DFMT_AUDIO_MPEG4,
  /* @} */

  /** \name Images (borrowed naming from FreeImage */
  /* @{ */
  DFMT_IMAGE_any_,                        /**< Image. */
  DFMT_IMAGE_BMP,               /**< Windows Bitmap */
  DFMT_IMAGE_ICO,               /**< Windows Icon */
  DFMT_IMAGE_ICN,               /**< Apple Icon. */
  DFMT_IMAGE_JPEG,                /**< JPEG */
  DFMT_IMAGE_JPEG2000,                /**< JPEG 2000 */
  DFMT_IMAGE_JPEG2000_ISO_IEC_15444_1,
  DFMT_IMAGE_JPEG2000_ISO_IEC_15444_2,
  DFMT_IMAGE_JNG,
  DFMT_IMAGE_KOALA,
  DFMT_IMAGE_LBM,
  DFMT_IMAGE_IFF,
  DFMT_IMAGE_MNG,
  DFMT_IMAGE_PBM,
  DFMT_IMAGE_PBMRAW,
  DFMT_IMAGE_PCD,
  DFMT_IMAGE_PCX,
  DFMT_IMAGE_PGM,
  DFMT_IMAGE_PGMRAW,
  DFMT_IMAGE_PNG,               /**< Portable Network Graphics */
  DFMT_IMAGE_PPM,
  DFMT_IMAGE_PPMRAW,
  DFMT_IMAGE_RAS,
  DFMT_IMAGE_TARGA,
  DFMT_IMAGE_TIFF,
  DFMT_IMAGE_WBMP,
  DFMT_IMAGE_PSD,
  DFMT_IMAGE_CUT,
  DFMT_IMAGE_XBM,
  DFMT_IMAGE_XPM,
  DFMT_IMAGE_DDS,
  DFMT_IMAGE_GIF,
  DFMT_IMAGE_HDR,
  DFMT_IMAGE_FAXG3,
  DFMT_IMAGE_SGI,
  DFMT_IMAGE_DJVU,             /**< DjVu Rasterization Image format. */
  DFMT_IMAGE_last = DFMT_IMAGE_DJVU,
  /* @} */

  /** \name Video */
  /* @{ */
  DFMT_VIDEO_any_,                  /**< Video. */
  DFMT_VIDEO_MATROSKA,              /**< Matroska. */
  DFMT_VIDEO_last = DFMT_VIDEO_MATROSKA,
  /* @} */

  /** \name Compressed files */
  /* @{ */
  DFMT_COMPRESSED_any_,
  DFMT_ZIP,
  DFMT_BZIP, DFMT_BZIP2, DFMT_GZIP,
  DFMT_TGZ, DFMT_TBZ,
  DFMT_TBZ2,
  DFMT_COMPRESSED_last = DFMT_TBZ2,
  DFMT_LZMA,
  DFMT_XZ,
  DFMT_7ZIP,
  /* @} */

  /** \name Archives */
  /* @{ */
  DFMT_ARCHIVE_any_,
  DFMT_AR, DFMT_TAR, DFMT_JAR, DFMT_RAR, DFMT_RPM,
  DFMT_ARCHIVE_last = DFMT_RPM,
  /* @} */

  /** \name File System and Disk Images */
  /* @{ */
  DFMT_ISO_9660,
  /* @} */

  /** \name XML files */
  /* @{ */
  DFMT_XML, DFMT_HTML, DFMT_MATHML, DFMT_SVG,
  /* @} */

  /** \name Tex/LaTeX */
  /* @{ */
  DFMT_TEX, DFMT_LATEX,
  /* @} */

  /** \name Text */
  /* @{ */
  DFMT_TEXT,
  DFMT_ASCII,
  DFMT_ASCII_EN,        ///< English
  DFMT_ISO8859,
  /* @} */

  /** \name Special text files. */
  /* @{ */
  DFMT_COPYING,
  DFMT_README,
  DFMT_INSTALL,
  DFMT_TODO,
  DFMT_NEWS,
  DFMT_AUTHORS,
  DFMT_WISHLIST,
  DFMT_HISTORY,
  DFMT_CREDITS,
  DFMT_FUTURE,
  DFMT_Changelog,
  /* @} */

  /** \name Makefile */
  /* @{ */
  DFMT_MAKEFILE,                /**< Makefile */
  DFMT_MAKEFILE_GNU,            /**< Makefile with GNU extensions. */
  /* @} */

  DFMT_CMAKEFILE,               /**< CMake Makefile */

  DFMT_AUTOCONF,                /**< Autoconf */
  DFMT_AUTOMAKE,                /**< Automake */

  DFMT_DOXYFILE,                /**< Doxyfile */

  DFMT_SCONS_SCONSTRUCT,        /**< SConstruct */

  DFMT_OMAKE_OMAKEFILE,         /**< OMakefile */

  /** \name Source and Bytecode */
  /* @{ */
  DFMT_SOURCE_CODE_any_,        /**< Source Code. */
  DFMT_BYTE_CODE_any_,          /**< Byte (Binary) Code. */

  DFMT_C,                       /**< C File. */
  DFMT_C_SOURCE,                /**< C Source File. */
  DFMT_C_HEADER,                /**< C Header File. */

  DFMT_CXX,                     /**< C++ File */
  DFMT_CXX_SOURCE,              /**< C++ Source File */
  DFMT_CXX_HEADER,              /**< C++ Header Fil */

  DFMT_D,                       /**< D File. */

  DFMT_JAVA_SOURCE,             /**< Java Source File */
  DFMT_JAVA_BYTECODE,           /**< Java Byte Source File */

  DFMT_LISP_SOURCE,             /**< Lisp Source Code */
  DFMT_EMACS_LISP_SOURCE,       /**< Emacs Lisp Source Code */
  DFMT_EMACS_LISP_BYTECODE,     /**< Emacs Lisp Bytecode */

  DFMT_PYTHON_SOURCE,           /**< Python Source Code */
  DFMT_PYTHON_BYTECODE,         /**< Python Byte Code */

  DFMT_RUBY_SOURCE,             /**< Ruby Source Code */
  DFMT_RUBY_BYTECODE,           /**< Ruby Source Code */
  /* @} */

  DFMT_SHELL,                   /**< Shell Script */
  DFMT_BASH,                    /**< BASH */
  DFMT_CSH,                     /**< CShell */
  DFMT_TCSH,                    /**< TCSH */

  /** \name Microsoft Office */
  /* @{ */
  DFMT_MSWORD,                  /**< Microsoft Word. */
  DFMT_MSEXCEL,                 /**< Microsoft Excel. */
  DFMT_MSPPOINT,                /**< Microsoft PowerPoint */
  /* @} */

} __attribute__ ((packed)) DFMT_t;

/*! Check if \p dfmt is a SPECIAL TEXT */
static inline bool DFMT_is_SPECIAL_TEXT(DFMT_t dfmt)
{
  return (dfmt == DFMT_COPYING ||
          dfmt == DFMT_README ||
          dfmt == DFMT_INSTALL ||
          dfmt == DFMT_TODO ||
          dfmt == DFMT_NEWS ||
          dfmt == DFMT_AUTHORS ||
          dfmt == DFMT_WISHLIST ||
          dfmt == DFMT_HISTORY ||
          dfmt == DFMT_CREDITS ||
          dfmt == DFMT_FUTURE ||
          dfmt == DFMT_Changelog);
}

/*! Check if \p dfmt is a TEXT */
static inline bool DFMT_is_TEXT(DFMT_t dfmt)
{
  return (dfmt == DFMT_TEXT ||
          dfmt == DFMT_ASCII ||
          dfmt == DFMT_ASCII_EN ||
          dfmt == DFMT_ISO8859);
}

/*! Check if \p dfmt is a ELF */
static inline bool DFMT_is_ELF(DFMT_t dfmt)
{
  return (dfmt == DFMT_ELF ||
          dfmt == DFMT_ELF_OBJ ||
          dfmt == DFMT_ELF_EXE);
}

/*! Check if \p dfmt is a XML-formatted */
static inline bool DFMT_is_XML(DFMT_t dfmt)
{
  return (dfmt == DFMT_XML ||
          dfmt == DFMT_HTML ||
          dfmt == DFMT_MATHML ||
          dfmt == DFMT_SVG);
}

/*! Check if \p dfmt is a JPEG Image. */
static inline bool DFMT_is_IMAGE_JPEG2000(DFMT_t dfmt)
{
  return (dfmt == DFMT_IMAGE_JPEG2000 ||
          dfmt == DFMT_IMAGE_JPEG2000_ISO_IEC_15444_1 ||
          dfmt == DFMT_IMAGE_JPEG2000_ISO_IEC_15444_2);
}

/*! Check if \p dfmt is an Rasterization Image. */
static inline bool DFMT_is_IMAGE_RASTER(DFMT_t dfmt)
{
  return (dfmt == DFMT_IMAGE_BMP ||
          dfmt == DFMT_IMAGE_ICO ||
          dfmt == DFMT_IMAGE_ICN ||
          dfmt == DFMT_IMAGE_JPEG ||
          DFMT_is_IMAGE_JPEG2000(dfmt) ||
          dfmt == DFMT_IMAGE_JNG ||
          dfmt == DFMT_IMAGE_KOALA ||
          dfmt == DFMT_IMAGE_LBM ||
          dfmt == DFMT_IMAGE_IFF ||
          dfmt == DFMT_IMAGE_MNG ||
          dfmt == DFMT_IMAGE_PBM ||
          dfmt == DFMT_IMAGE_PBMRAW ||
          dfmt == DFMT_IMAGE_PCD ||
          dfmt == DFMT_IMAGE_PCX ||
          dfmt == DFMT_IMAGE_PGM ||
          dfmt == DFMT_IMAGE_PGMRAW ||
          dfmt == DFMT_IMAGE_PNG ||
          dfmt == DFMT_IMAGE_PPM ||
          dfmt == DFMT_IMAGE_PPMRAW ||
          dfmt == DFMT_IMAGE_RAS ||
          dfmt == DFMT_IMAGE_TARGA ||
          dfmt == DFMT_IMAGE_TIFF ||
          dfmt == DFMT_IMAGE_WBMP ||
          dfmt == DFMT_IMAGE_PSD ||
          dfmt == DFMT_IMAGE_CUT ||
          dfmt == DFMT_IMAGE_XBM ||
          dfmt == DFMT_IMAGE_XPM ||
          dfmt == DFMT_IMAGE_DDS ||
          dfmt == DFMT_IMAGE_GIF ||
          dfmt == DFMT_IMAGE_HDR ||
          dfmt == DFMT_IMAGE_FAXG3 ||
          dfmt == DFMT_IMAGE_SGI ||
          dfmt == DFMT_IMAGE_DJVU);
}

/*! Check if \p dfmt is a Vector Image. */
static inline bool DFMT_is_IMAGE_VECTOR(DFMT_t dfmt)
{
  return (dfmt == DFMT_SVG);
}

/*! Check if \p dfmt is an Image. */
static inline bool DFMT_is_IMAGE(DFMT_t dfmt)
{
  return (DFMT_is_IMAGE_RASTER(dfmt) ||
          DFMT_is_IMAGE_VECTOR(dfmt));
}

/*! Check if \p dfmt is an Video. */
static inline bool DFMT_is_VIDEO(DFMT_t dfmt)
{
  return (dfmt >= DFMT_VIDEO_any_ &&
          dfmt <= DFMT_VIDEO_last);
}

/*! Check if \p dfmt is a COMPRESSED */
static inline bool DFMT_is_COMPRESSED(DFMT_t dfmt)
{
  return (dfmt >= DFMT_COMPRESSED_any_ &&
          dfmt <= DFMT_COMPRESSED_last);
}

/*! Check if \p dfmt is a ARCHIVE */
static inline bool DFMT_is_ARCHIVE(DFMT_t dfmt)
{
  return (dfmt >= DFMT_ARCHIVE_any_ &&
          dfmt <= DFMT_ARCHIVE_last);
}

/*! Check if \p dfmt is a SOURCE CODE */
static inline bool DFMT_is_SOURCE_CODE(DFMT_t dfmt)
{
  return (dfmt == DFMT_SOURCE_CODE_any_ ||
          dfmt == DFMT_C_SOURCE ||
          dfmt == DFMT_C_HEADER ||
          dfmt == DFMT_CXX_SOURCE ||
          dfmt == DFMT_CXX_HEADER ||
          dfmt == DFMT_JAVA_SOURCE ||
          dfmt == DFMT_LISP_SOURCE ||
          dfmt == DFMT_EMACS_LISP_SOURCE ||
          dfmt == DFMT_PYTHON_SOURCE);
}

/*! Check if \p dfmt is a HEADER SOURCE CODE file (for compiled languages) */
static inline bool DFMT_is_HEADER_SOURCE_CODE(DFMT_t dfmt)
{
  return (dfmt == DFMT_C_HEADER ||
          dfmt == DFMT_CXX_HEADER);
}

/*! Check if \p dfmt is a BYTE CODE (compiled) */
static inline bool DFMT_is_BYTE_CODE(DFMT_t dfmt)
{
  return (dfmt == DFMT_SOURCE_CODE_any_ ||
          dfmt == DFMT_JAVA_BYTECODE ||
          dfmt == DFMT_EMACS_LISP_BYTECODE ||
          dfmt == DFMT_PYTHON_BYTECODE);
}

/*! Check if \p dfmt is a AUDIO (compiled) */
static inline bool DFMT_is_AUDIO(DFMT_t dfmt)
{
  return (dfmt >= DFMT_AUDIO_any_ &&
          dfmt <= DFMT_AUDIO_last);
}

/*! Check if \p dfmt is a MEDIA (compiled) */
static inline bool DFMT_is_MEDIA(DFMT_t dfmt)
{
  return (DFMT_is_AUDIO(dfmt) ||
          DFMT_is_IMAGE(dfmt) ||
          DFMT_is_VIDEO(dfmt));
}

/*! Check if \p dfmt is a BINARY */
static inline bool DFMT_is_BINARY(DFMT_t dfmt)
{
  return (dfmt == DFMT_BINARY_any_ ||
          DFMT_is_ELF(dfmt) ||
          DFMT_is_COMPRESSED(dfmt) ||
          DFMT_is_ARCHIVE(dfmt) ||
          DFMT_is_BYTE_CODE(dfmt));
}

/*! Lookup name of \p dfmt as a constant static string.
 * \return the looked up name.
 */
const char * DFMT_getName(DFMT_t dfmt);

/*! Lookup a comma-separated list of program names that operates on
 * files of type \p dfmt as a constant static string.
 * \return the looked up program name.
 */
const char * DFMT_getOps(DFMT_t dfmt);

/*! Specialize \p dfmt using local file name path \p pathL.
 * \return the specialized \p dfmt.
 */
DFMT_t DFMT_refine_from_pathL(DFMT_t dfmt,
                              const xStr * pathL);

/*! Specialize \p dfmt using file \p magic.
 * \return the specialized \p dfmt.
 */
DFMT_t DFMT_refine_from_magic(DFMT_t dfmt,
                              const xStr * magic);

bool DFMT_match(DFMT_t fmt, DFMT_t fmt_mask);

#ifdef __cplusplus
}
#endif
