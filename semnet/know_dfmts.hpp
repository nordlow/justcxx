/*! \file know_dfmts.hpp
 * \brief File Type Magic Headers Patterns.
 * \author Copyright (C) 2012 Per Nordlöw (per.nordlow@gmail.com)
 * \date 2009-03-17 15:01
 */

#pragma once
#include "patt.hpp"

namespace semnet {

namespace filesystem {
class FileType;
class DirType;
}

namespace patterns {

class Lit;
class Sit;
class Alt;
class Any;
class Rep;
class Seq;
class Sit;
class Lang;

///< Load All Specifications.
void learn_dfmts();

/* ---------------------------- Group Separator ---------------------------- */

using filesystem::FileType;
using filesystem::DirType;

extern Base * gp_git_URL_suffix;

extern DirType* gp_GitDir;
extern Seq* gp_GitDirName;

extern Base * gp_VCSDirName;
extern DirType * gp_VCSDir;
extern DirType * gp_VCDir;

///< Load Specifications for Directory Types.
void learn_dfmts_dirtypes();

/* ---------------------------- Group Separator ---------------------------- */

extern Base *gp_C_hdr_fname;
extern Base *gp_C_src_fname;
extern Base *gp_C_fname;

extern Base *gp_Cxx_hdr_fname;
extern Base *gp_Cxx_src_fname;
extern Base *gp_Cxx_fname;

extern Base *gp_Java_src_fname;
extern Base *gp_Java_Class_src_fname;

extern Base *gp_Python_src_fname;
extern Base *gp_Python_Bytecode_fname;

extern Base *gp_Shell_fname;

extern Base *gp_Bash_fname;
extern Base *gp_Csh_fname;

extern Base *gp_Makefile_fname;

extern Base *gp_Elisp_src_fname;
extern Base *gp_Elisp_Bytecode_fname;

extern FileType * gp_C_ftype;
extern FileType * gp_Cxx_ftype;
extern FileType * gp_Java_Source_ftype;
extern FileType * gp_Java_Class_ftype;
extern FileType * gp_Python_ftype;
extern FileType * gp_Python_Bytecode_ftype;
extern FileType * gp_ftype_Elisp;
extern FileType * gp_Elisp_Bytecode_ftype;
extern FileType * gp_ftype_Shell;
extern FileType * gp_Csh_ftype;
extern FileType * gp_Bash_ftype;
extern FileType * gp_Make_ftype;

///< Load Specifications for Source Code formats.
void learn_dfmt_SourceCode();

/* ---------------------------- Group Separator ---------------------------- */

extern Base * gp_ELF_magic;
extern Seq * gp_ELF_dfmt;

extern FileType * gp_ELF_ftype;

///< Load Specification for the \em ELF file format.
void learn_dfmt_ELF();

///< Load Specification for the \em ID3 file format.
void learn_dfmt_ID3();

///< Load Specification for the \em APE file format.
void learn_dfmt_APE();

extern Alt * gp_audio_dfmt;

///< Load Specification for \em Audio formats.
void learn_dfmt_Audio();

/* ---------------------------- Group Separator ---------------------------- */

extern Base * gp_tar_posix_magic;
extern Seq * gp_tar_posix_dfmt;

extern Base * gp_Z_dfmt;
extern Base * gp_gz_dfmt;
extern Base * gp_bz_dfmt;
extern Base * gp_xz_dfmt;
extern Base * gp_lzx_dfmt;
extern Base * gp_lzma_dfmt;
extern Base * gp_zip_dfmt;
extern Base * gp_compress_dfmt;

/* ---------------------------- Group Separator ---------------------------- */

}
}
