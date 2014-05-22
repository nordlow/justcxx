#include "dfmt.h"
#include "substr_match.h"

const char * DFMT_getName(DFMT_t fmt)
{
  const char * str = NULL;
  switch (fmt) {

  case DFMT_any_: str = "Any"; break;

  case DFMT_BINARY_any_: str = "Binary file"; break;

  case DFMT_ELF: str = "Executable and Linkable Format (ELF)"; break;
  case DFMT_ELF_OBJ: str = "ELF Object"; break;
  case DFMT_ELF_EXE: str = "ELF Executable"; break;

  case DFMT_LIBTOOL_LIBRARY_FILE: str = "Libtool library file"; break;

  case DFMT_IMAGE_any_: str = "Image"; break;
  case DFMT_IMAGE_BMP: str = "Image BMP"; break;
  case DFMT_IMAGE_ICO: str = "Image ICO"; break;
  case DFMT_IMAGE_ICN: str = "Image ICN"; break;
  case DFMT_IMAGE_JPEG: str = "Image JPEG"; break;
  case DFMT_IMAGE_JPEG2000: str = "Image JPEG2000"; break;
  case DFMT_IMAGE_JPEG2000_ISO_IEC_15444_1: str = "Image JPEG2000_ISO_IEC_15444_1"; break;
  case DFMT_IMAGE_JPEG2000_ISO_IEC_15444_2: str = "Image JPEG2000_ISO_IEC_15444_2"; break;
  case DFMT_IMAGE_JNG: str = "Image JNG"; break;
  case DFMT_IMAGE_KOALA: str = "Image KOALA"; break;
  case DFMT_IMAGE_LBM: str = "Image LBM"; break;
  case DFMT_IMAGE_IFF: str = "Image IFF"; break;
  case DFMT_IMAGE_MNG: str = "Image MNG"; break;
  case DFMT_IMAGE_PBM: str = "Image PBM"; break;
  case DFMT_IMAGE_PBMRAW: str = "Image PBMRAW"; break;
  case DFMT_IMAGE_PCD: str = "Image PCD"; break;
  case DFMT_IMAGE_PCX: str = "Image PCX"; break;
  case DFMT_IMAGE_PGM: str = "Image PGM"; break;
  case DFMT_IMAGE_PGMRAW: str = "Image PGMRAW"; break;
  case DFMT_IMAGE_PNG: str = "Image PNG"; break;
  case DFMT_IMAGE_PPM: str = "Image PPM"; break;
  case DFMT_IMAGE_PPMRAW: str = "Image PPMRAW"; break;
  case DFMT_IMAGE_RAS: str = "Image RAS"; break;
  case DFMT_IMAGE_TARGA: str = "Image TARGA"; break;
  case DFMT_IMAGE_TIFF: str = "Image TIFF"; break;
  case DFMT_IMAGE_WBMP: str = "Image WBMP"; break;
  case DFMT_IMAGE_PSD: str = "Image PSD"; break;
  case DFMT_IMAGE_CUT: str = "Image CUT"; break;
  case DFMT_IMAGE_XBM: str = "Image XBM"; break;
  case DFMT_IMAGE_XPM: str = "Image XPM"; break;
  case DFMT_IMAGE_DDS: str = "Image DDS"; break;
  case DFMT_IMAGE_GIF: str = "Image GIF"; break;
  case DFMT_IMAGE_HDR: str = "Image HDR"; break;
  case DFMT_IMAGE_FAXG3: str = "Image FAXG3"; break;
  case DFMT_IMAGE_SGI: str = "Image SGI"; break;
  case DFMT_IMAGE_DJVU: str = "DjVu"; break;

  case DFMT_ZIP: str = "Zip"; break;
  case DFMT_BZIP: str = "BZip"; break;
  case DFMT_TGZ: str = "tgz"; break;
  case DFMT_BZIP2: str = "BZip2"; break;
  case DFMT_GZIP: str = "Gzip"; break;
  case DFMT_TBZ: str = "tbz"; break;
  case DFMT_TBZ2: str = "tbz2"; break;

  case DFMT_AR: str = "ar"; break;
  case DFMT_TAR: str = "tar"; break;
  case DFMT_JAR: str = "jar"; break;
  case DFMT_RAR: str = "rar"; break;

  case DFMT_RPM: str = "Redhat Package Manager Package"; break;
  case DFMT_HTML: str = "HTML"; break;
  case DFMT_MATHML: str = "MATHML"; break;
  case DFMT_SVG: str = "SVG"; break;
  case DFMT_LATEX: str = "LaTeX"; break;

  case DFMT_ISO_9660: str = "DFMT_ISO_9660"; break;

  case DFMT_XML: str = "DFMT_XML"; break;

  case DFMT_TEX: str = "DFMT_TEX"; break;

  case DFMT_TEXT: str = "DFMT_TEXT"; break;
  case DFMT_ASCII: str = "ASCII"; break;
  case DFMT_ASCII_EN: str = "ASCII English"; break;
  case DFMT_ISO8859: str = "DFMT_ISO8859"; break;

  case DFMT_COPYING: str = "COPYING text file"; break;
  case DFMT_README: str = "README text file"; break;
  case DFMT_INSTALL: str = "INSTALL text file"; break;
  case DFMT_TODO: str = "TODO text file"; break;
  case DFMT_NEWS: str = "NEWS text file"; break;
  case DFMT_AUTHORS: str = "AUTHORS text file"; break;
  case DFMT_WISHLIST: str = "WISHLIST text file"; break;
  case DFMT_HISTORY: str = "HISTORY text file"; break;
  case DFMT_CREDITS: str = "CREDITS text file"; break;
  case DFMT_FUTURE: str = "FUTURE text file"; break;
  case DFMT_Changelog: str = "Changelog text file"; break;

  case DFMT_MAKEFILE: str = "Makefile"; break;
  case DFMT_MAKEFILE_GNU: str = "GNU Makefile"; break;

  case DFMT_CMAKEFILE: str = "CMake Makefile"; break;

  case DFMT_AUTOCONF: str = "Autoconf Configuration file"; break;
  case DFMT_AUTOMAKE: str = "Automake Makefile"; break;

  case DFMT_C: str = "C Code"; break;
  case DFMT_C_SOURCE: str = "C Source Code"; break;
  case DFMT_C_HEADER: str = "C Header Code"; break;

  case DFMT_CXX: str = "C++ Code"; break;
  case DFMT_CXX_SOURCE: str = "C++ Source Code"; break;
  case DFMT_CXX_HEADER: str = "C++ Header Code"; break;

  case DFMT_JAVA_SOURCE: str = "Java Source Code"; break;
  case DFMT_JAVA_BYTECODE: str = "Java Byte Code"; break;

  case DFMT_LISP_SOURCE: str = "Lisp Source Code"; break;
  case DFMT_EMACS_LISP_SOURCE: str = "Emacs Lisp Source Code"; break;
  case DFMT_EMACS_LISP_BYTECODE: str = "Emacs Lisp Bytecode"; break;

  case DFMT_PYTHON_SOURCE: str = "Python Source Code"; break;
  case DFMT_PYTHON_BYTECODE: str = "Python Bytecode"; break;

  case DFMT_RUBY_SOURCE: str = "Ruby Source Code"; break;
  case DFMT_RUBY_BYTECODE: str = "Ruby Bytecode"; break;

  case DFMT_MSWORD: str = "Microsoft Word Document"; break;
  case DFMT_MSEXCEL: str = "Microsoft Excel Document"; break;
  case DFMT_MSPPOINT: str = "Microsoft Powerpoint Document"; break;

  case DFMT_DOXYFILE: str = "Doxygen Configuration File (Doxyfile)"; break;
  case DFMT_SCONS_SCONSTRUCT: str = "SCons Build File (SConstruct)"; break;
  case DFMT_OMAKE_OMAKEFILE: str = "OMake Build File (OMakefile)"; break;

  case DFMT_SHELL: str = "Shell Script Code"; break;
  case DFMT_BASH: str = "BASH Script Code"; break;
  case DFMT_CSH: str = "CShell Script Code"; break;
  case DFMT_TCSH: str = "TSCH Script Code"; break;
  }
  return str;
}

DFMT_t DFMT_refine_from_pathL(DFMT_t fmt,
                              const xStr * pathL)
{
  /* \todo Optimize using Hash Table */
  if      (xstr_beg(pathL, "COPYING", 7)) { fmt = DFMT_COPYING; }

  else if (xstr_beg(pathL, "README", 6)) { fmt = DFMT_README; }
  else if (xstr_beg(pathL, "Readme", 6)) { fmt = DFMT_README; }

  else if (xstr_beg(pathL, "INSTALL", 7)) { fmt = DFMT_INSTALL; }
  else if (xstr_beg(pathL, "TODO", 4)) { fmt = DFMT_TODO; }
  else if (xstr_beg(pathL, "NEWS", 4)) { fmt = DFMT_NEWS; }
  else if (xstr_beg(pathL, "AUTHORS", 7)) { fmt = DFMT_AUTHORS; }
  else if (xstr_beg(pathL, "WISHLIST", 8)) { fmt = DFMT_WISHLIST; }
  else if (xstr_beg(pathL, "HISTORY", 7)) { fmt = DFMT_HISTORY; }
  else if (xstr_beg(pathL, "CREDITS", 7)) { fmt = DFMT_CREDITS; }
  else if (xstr_beg(pathL, "FUTURE", 7)) { fmt = DFMT_FUTURE; }
  else if (xstr_beg(pathL, "Changelog", 9)) { fmt = DFMT_Changelog; }

  else if (xstr_beg(pathL, "Makefile", 8)) { fmt = DFMT_MAKEFILE; }
  else if (xstr_beg(pathL, "makefile", 8)) { fmt = DFMT_MAKEFILE; }
  else if (xstr_beg(pathL, "GNUmakefile", 8)) { fmt = DFMT_MAKEFILE_GNU; }
  else if (xstr_end(pathL, ".mk", 3)) { fmt = DFMT_MAKEFILE; }
  else if (xstr_end(pathL, ".cmk", 4)) { fmt = DFMT_CMAKEFILE; }

  else if (xstr_beg(pathL, "Doxyfile", 8)) { fmt = DFMT_DOXYFILE; }
  else if (xstr_beg(pathL, "OMakefile", 9)) { fmt = DFMT_OMAKE_OMAKEFILE; }

  else if (xstr_beg(pathL, "configure.in", 12)) { fmt = DFMT_AUTOCONF; }
  else if (xstr_beg(pathL, "configure.ac", 12)) { fmt = DFMT_AUTOCONF; }

  else if (xstr_beg(pathL, "Makefile.am", 11)) { fmt = DFMT_AUTOMAKE; }

  else if (xstr_iend(pathL, ".htm", 4)) { fmt = DFMT_HTML; }
  else if (xstr_iend(pathL, ".html", 5)) { fmt = DFMT_HTML; }

  else if (xstr_iend(pathL, ".txt", 4)) { fmt = DFMT_TEXT; }

  /* image formats extensions */
  else if (xstr_iend(pathL, ".bmp", 4)) { fmt = DFMT_IMAGE_BMP; }
  else if (xstr_iend(pathL, ".ico", 4))  { fmt = DFMT_IMAGE_ICO; }
  else if (xstr_iend(pathL, ".icn", 4))  { fmt = DFMT_IMAGE_ICN; }

  else if (xstr_iend(pathL, ".jpeg", 5))  { fmt = DFMT_IMAGE_JPEG; }
  else if (xstr_iend(pathL, ".jpg", 4))  { fmt = DFMT_IMAGE_JPEG; }

  else if (xstr_iend(pathL, ".j2k", 4))  { fmt = DFMT_IMAGE_JPEG2000; }
  else if (xstr_iend(pathL, ".jp2", 4))  { fmt = DFMT_IMAGE_JPEG2000_ISO_IEC_15444_1; }
  else if (xstr_iend(pathL, ".jpx", 4))  { fmt = DFMT_IMAGE_JPEG2000_ISO_IEC_15444_2; }

  else if (xstr_iend(pathL, ".jng", 4))  { fmt = DFMT_IMAGE_JNG; }

  else if (xstr_iend(pathL, ".koala", 6))  { fmt = DFMT_IMAGE_KOALA; }
  else if (xstr_iend(pathL, ".lbm", 4))  { fmt = DFMT_IMAGE_LBM; }
  else if (xstr_iend(pathL, ".iff", 4))  { fmt = DFMT_IMAGE_IFF; }
  else if (xstr_iend(pathL, ".mng", 4))  { fmt = DFMT_IMAGE_MNG; }

  else if (xstr_iend(pathL, ".pcd", 4))  { fmt = DFMT_IMAGE_PCD; }
  else if (xstr_iend(pathL, ".pcx", 4))  { fmt = DFMT_IMAGE_PCX; }

  else if (xstr_iend(pathL, ".pbm", 4))  { fmt = DFMT_IMAGE_PBM; }
  else if (xstr_iend(pathL, ".pbmraw", 7))  { fmt = DFMT_IMAGE_PBMRAW; }

  else if (xstr_iend(pathL, ".pgm", 4))  { fmt = DFMT_IMAGE_PGM; }
  else if (xstr_iend(pathL, ".pgmraw", 7))  { fmt = DFMT_IMAGE_PGMRAW; }

  else if (xstr_iend(pathL, ".ppm", 4))  { fmt = DFMT_IMAGE_PPM; }
  else if (xstr_iend(pathL, ".ppmraw", 7))  { fmt = DFMT_IMAGE_PPMRAW; }

  else if (xstr_iend(pathL, ".png", 4))  { fmt = DFMT_IMAGE_PNG; }

  else if (xstr_iend(pathL, ".ras", 4))  { fmt = DFMT_IMAGE_RAS; }
  else if (xstr_iend(pathL, ".targa", 6))  { fmt = DFMT_IMAGE_TARGA; }
  else if (xstr_iend(pathL, ".tiff", 5))  { fmt = DFMT_IMAGE_TIFF; }
  else if (xstr_iend(pathL, ".wbmp", 5))  { fmt = DFMT_IMAGE_WBMP; }
  else if (xstr_iend(pathL, ".psd", 4))  { fmt = DFMT_IMAGE_PSD; }
  else if (xstr_iend(pathL, ".cut", 4))  { fmt = DFMT_IMAGE_CUT; }
  else if (xstr_iend(pathL, ".xbm", 4))  { fmt = DFMT_IMAGE_XBM; }
  else if (xstr_iend(pathL, ".xpm", 4))  { fmt = DFMT_IMAGE_XPM; }
  else if (xstr_iend(pathL, ".dds", 4))  { fmt = DFMT_IMAGE_DDS; }
  else if (xstr_iend(pathL, ".gif", 4))  { fmt = DFMT_IMAGE_GIF; }
  else if (xstr_iend(pathL, ".hdr", 4))  { fmt = DFMT_IMAGE_HDR; }
  else if (xstr_iend(pathL, ".faxg3", 6))  { fmt = DFMT_IMAGE_FAXG3; }
  else if (xstr_iend(pathL, ".sgi", 4))  { fmt = DFMT_IMAGE_SGI; }
  else if (xstr_iend(pathL, ".djvu", 5))  { fmt = DFMT_IMAGE_DJVU; }
  else if (xstr_iend(pathL, ".djv", 4))  { fmt = DFMT_IMAGE_DJVU; }

  else if (xstr_end(pathL, ".a", 2)) { fmt = DFMT_AR; }
  else if (xstr_end(pathL, ".tar", 4)) { fmt = DFMT_TAR; }

  else if (xstr_end(pathL, ".la", 3)) { fmt = DFMT_LIBTOOL_LIBRARY_FILE; }

  else if (xstr_end(pathL, ".zip", 4)) { fmt = DFMT_ZIP; }

  else if (xstr_iend(pathL, ".gz", 3)) { fmt = DFMT_GZIP; }
  else if (xstr_iend(pathL, ".bz", 3)) { fmt = DFMT_BZIP; }
  else if (xstr_iend(pathL, ".bz2", 4)) { fmt = DFMT_BZIP2; }

  else if (xstr_iend(pathL, ".tgz", 4)) { fmt = DFMT_TGZ; }
  else if (xstr_iend(pathL, ".tbz", 4)) { fmt = DFMT_TBZ2; }
  else if (xstr_iend(pathL, ".tbz2", 5)) { fmt = DFMT_TBZ2; }

  else if (xstr_iend(pathL, ".rar", 4)) { fmt = DFMT_RAR; }
  else if (xstr_iend(pathL, ".rpm", 4)) { fmt = DFMT_RPM; }

  else if ( xstr_end(pathL, ".c", 2)) { fmt = DFMT_C_SOURCE; }
  else if ( xstr_end(pathL, ".h", 2)) { fmt = DFMT_C_HEADER; }

  else if ( xstr_end(pathL, ".c++", 4)) { fmt = DFMT_CXX_SOURCE; }
  else if ( xstr_end(pathL, ".h++", 4)) { fmt = DFMT_CXX_HEADER; }
  else if ( xstr_end(pathL, ".C", 2)) { fmt = DFMT_CXX_SOURCE; }
  else if ( xstr_end(pathL, ".H", 2)) { fmt = DFMT_CXX_HEADER; }
  else if ( xstr_end(pathL, ".cc", 3)) { fmt = DFMT_CXX_SOURCE; }
  else if ( xstr_end(pathL, ".hh", 3)) { fmt = DFMT_CXX_HEADER; }

  else if ( xstr_end(pathL, ".cpp", 4)) { fmt = DFMT_CXX_SOURCE; }
  else if ( xstr_end(pathL, ".hpp", 4)) { fmt = DFMT_CXX_HEADER; }

  else if ( xstr_end(pathL, ".java", 5)) { fmt = DFMT_JAVA_SOURCE; }

  return fmt;
}

DFMT_t DFMT_refine_from_magic(DFMT_t fmt,
                              const xStr * magic)
{
  /* \todo Optimize using Hash Table */
  if      (xstr_beg(magic, "image data", 5+1+4)) { fmt = DFMT_IMAGE_any_; }

  if      (xstr_beg(magic, "ASCII", 5)) { fmt = DFMT_ASCII; }
  else if (xstr_beg(magic, "ASCII make commands text", 24)) { fmt = DFMT_MAKEFILE; }

  else if (xstr_beg(magic, "ELF", 3)) { fmt = DFMT_ELF; }

  else if (xstr_beg(magic, "libtool library file", 20)) { fmt = DFMT_LIBTOOL_LIBRARY_FILE; }

  else if (xstr_beg(magic, "XML", 3)) { fmt = DFMT_XML; }
  else if (xstr_beg(magic, "HTML", 4)) { fmt = DFMT_HTML; }

  else if (xstr_beg(magic, "JPEG", 4)) { fmt = DFMT_IMAGE_JPEG; }
  else if (xstr_beg(magic, "JPEG 2000", 4+1+4)) { fmt = DFMT_IMAGE_JPEG2000; }
  else if (xstr_beg(magic, "TIFF", 4)) { fmt = DFMT_IMAGE_TIFF; }
  else if (xstr_beg(magic, "GIF", 3)) { fmt = DFMT_IMAGE_GIF; }
  else if (xstr_beg(magic, "PNG", 3)) { fmt = DFMT_IMAGE_PNG; }

  else if (xstr_beg(magic, "Zip", 3)) { fmt = DFMT_ZIP; }
  else if (xstr_beg(magic, "gzip", 4)) { fmt = DFMT_GZIP; }
  else if (xstr_beg(magic, "bzip", 4)) { fmt = DFMT_BZIP2; }

  else if (xstr_beg(magic, "rar", 3)) { fmt = DFMT_RAR; }
  else if (xstr_beg(magic, "current ar archive", 18)) { fmt = DFMT_AR; }
  else if (xstr_beg(magic, "POSIX tar archive", 16)) { fmt = DFMT_TAR; }

  else if (xstr_beg(magic, "RPM", 3)) { fmt = DFMT_RPM; }

  return fmt;
}

const char * DFMT_getOps(DFMT_t fmt)
{
  const char * str = NULL;
  switch (fmt) {

  case DFMT_MAKEFILE: str = "make"; break;
  case DFMT_MAKEFILE_GNU: str = "make, gmake"; break;

  case DFMT_CMAKEFILE: str = "cmake, ccmake"; break;

  case DFMT_AUTOCONF: str = "autoconf"; break;
  case DFMT_AUTOMAKE: str = "automake"; break;

  case DFMT_JAVA_SOURCE: str = "javac <1>, java <1>"; break;

  case DFMT_PYTHON_SOURCE: str = "python <1>"; break;

  case DFMT_MSWORD: str = "msword.exe"; break;
  case DFMT_MSEXCEL: str = "msexcel.exe"; break;
  case DFMT_MSPPOINT: str = "msppoint.exe"; break;

  case DFMT_DOXYFILE: str = "doxygen <1>"; break;
  case DFMT_SCONS_SCONSTRUCT: str = "scons <1>"; break;
  case DFMT_OMAKE_OMAKEFILE: str = "omake <1>"; break;

  case DFMT_RAR: str = "rar <1>, unrar <1>"; break;
  case DFMT_AR: str = "ar <1>"; break;
  case DFMT_TAR: str = "tar <1>"; break;

  case DFMT_ZIP: str = "zip <1>, unzip <1>"; break;
  case DFMT_GZIP: str = "gzip <1>, gunzip <1>"; break;
  case DFMT_BZIP2: str = "bzip2 <1>, bzip2recover <1>"; break;

  default: break;
  }
  return str;
}

bool DFMT_match(DFMT_t fmt, DFMT_t fmt_mask)
{
  bool ok = false;
  switch (fmt_mask) {
  case DFMT_BINARY_any_: ok = DFMT_is_BINARY(fmt); break;
  case DFMT_IMAGE_any_: ok = DFMT_is_IMAGE(fmt); break;
  case DFMT_AUDIO_any_: ok = DFMT_is_AUDIO(fmt); break;
  case DFMT_VIDEO_any_: ok = DFMT_is_VIDEO(fmt); break;
  case DFMT_COMPRESSED_any_: ok = DFMT_is_COMPRESSED(fmt); break;
  case DFMT_ARCHIVE_any_: ok = DFMT_is_ARCHIVE(fmt); break;
  case DFMT_SOURCE_CODE_any_: ok = DFMT_is_SOURCE_CODE(fmt); break;
  case DFMT_BYTE_CODE_any_: ok = DFMT_is_BYTE_CODE(fmt); break;
  }
  return ok;
}
