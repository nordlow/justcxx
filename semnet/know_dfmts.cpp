#include "lit.hpp"
#include "alt.hpp"
#include "ran.hpp"
#include "sit.hpp"
#include "rep.hpp"
#include "any.hpp"
#include "seq.hpp"
#include "comb.hpp"
#include "ctx.hpp"
#include "filetype.hpp"
#include "filex.hpp"
#include "dirtype.hpp"
#include "lang.hpp"

#include "know_common.hpp"
#include "know_dfmts.hpp"
#include "know_langC.hpp"
#include "know_langD.hpp"
#include "know_langCxx.hpp"
#include "know_langLaTeX.hpp"

#include <elf.h>

/* ---------------------------- Group Separator ---------------------------- */

namespace semnet {
namespace patterns {

using filesystem::FileType;
using filesystem::DirType;

Base * gp_git_URL_suffix = nullptr;
Seq *  gp_GitDirName     = nullptr;
Base * gp_VCSDirName     = nullptr;

DirType * gp_GitDir = nullptr;
DirType * gp_VCSDir = nullptr;
DirType * gp_VCDir  = nullptr;

void learn_dfmts_dirtypes()
{
    using namespace filesystem::gen;
    using namespace patterns::gen;

    /* Version Control Directory Name */
    {
        /* Compare with Emacs 23.1
         * - vc-directory-exclusion-list
         *   ("SCCS", "RCS", "CVS", "MCVS", ".svn", ".git", ".hg", ".bzr", "_MTN", "_darcs", "{arch}")
         * - vc-handled-backends
         *   (DARCS git RCS CVS SVN SCCS Bzr Git Hg Mtn Arch)
         */
        const char * vc_dirnameA[] = { "SCCS", "RCS", "CVS", "MCVS",
                                       ".svn", ".git", ".hg", ".bzr",
                                       "_MTN", "_darcs", "{arch}" };
        const char * vc_nameA[] = { "SCCS", "RCS", "CVS", "MCVS",
                                    "Subversion", "Git", "Mercurial", "Bzr (Bazaar)",
                                    "Monotone", "Darcs", "Arch" };
        const size_t vc_dirN = ARRAY_LENGTH(vc_dirnameA);
        const size_t vc_nameN = ARRAY_LENGTH(vc_nameA);
        if (vc_dirN != vc_nameN) { PERR("vc_dirN != vc_nameN\n"); }
        Base * vc_dir_ppA[vc_nameN];

        for (size_t i = 0; i < vc_nameN; ++i)
        {
            Base * ppatt_i = ctx(PCTX_IN_FILE_NAME, lit_full(vc_dirnameA[i]))->set_name(vc_nameA[i]); /* set name */
            vc_dir_ppA[i] = ppatt_i;
        }

        gp_VCSDirName = alt(vc_dir_ppA, vc_nameN)->set_name("Version Control System (VCS) Database Directory Name");

        gp_VCSDir = new DirType(gp_VCSDirName, nullptr,
                                "Version Control System (VCS) Database Directory"
                                "Directory Containing Database for a Version Control System (VCS)");

        gp_VCDir = new DirType(gp_VCSDirName, nullptr,
                               "Version Controlled Directory"
                               "Directory Containing Database for a Version Control System (VCS)");
        gp_VCDir->add_ops({ OP_VCS_STATUS, OP_VCS_COMMIT, OP_VCS_UPDATE});

        gp_git_URL_suffix = fnx("git");
    }

    // GIT Directory
    {
        // \todo All* gp_GitDir_SubDirs_ftype = nullptr; containing CHILDs of type Lit
        // \todo All* gp_GitDir_Files_ftype = nullptr;
        gp_GitDir = nullptr;

        // \todo gp_GitDir_SubDirs = REL_CHILD => lit{ "config", "description", "HEAD", "hooks", "info", "objects" };
        // \todo gp_GitDir_Files = REL_CHILD => lit{ "branches", "refs" };
        gp_GitDirName = lit_full(".git");
        gp_GitDir = new DirType(gp_GitDirName, nullptr,
                                "GIT Database",
                                "Directory Containing Database for the GIT Version Control System (VCS)");
    }
}

/* ---------------------------- Group Separator ---------------------------- */

// C
Base *gp_C_hdr_fname = nullptr;
Base *gp_C_src_fname = nullptr;
Base *gp_C_fname = nullptr;
// D
Base *gp_D_fname = nullptr;
// C++
Base *gp_Cxx_hdr_fname = nullptr;
Base *gp_Cxx_src_fname = nullptr;
Base *gp_Cxx_fname = nullptr;
// Java
Base *gp_Java_src_fname = nullptr;
Base *gp_Java_Class_src_fname = nullptr;
// Python
Base *gp_Python_src_fname = nullptr;
Base *gp_Python_Bytecode_fname = nullptr;
// Shell
Base *gp_Shell_fname = nullptr;
Base *gp_Bash_fname = nullptr;
Base *gp_Csh_fname = nullptr;
// Makefile
Base *gp_Makefile_fname = nullptr;
Base * gp_Makefile_target = nullptr;
// Emacs-Lisp
Base *gp_Elisp_src_fname = nullptr;
Base *gp_Elisp_Bytecode_fname = nullptr;

FileType * gp_C_ftype = nullptr;
FileType * gp_Cxx_ftype = nullptr;
FileType * gp_D_ftype = nullptr;
FileType * gp_Java_Source_ftype = nullptr;
FileType * gp_Java_Class_ftype = nullptr;
FileType * gp_Python_ftype = nullptr;
FileType * gp_Python_Bytecode_ftype = nullptr;
FileType * gp_Elisp_ftype = nullptr;
FileType * gp_Elisp_Bytecode_ftype = nullptr;
FileType * gp_Shell_ftype = nullptr;
FileType * gp_Bash_ftype = nullptr;
FileType * gp_Csh_ftype = nullptr;
FileType * gp_Make_ftype = nullptr;

Base* gp_Julia_fname = nullptr;
Base* gp_Matlab_fname = nullptr;
Base* gp_Octave_fname = nullptr;

Base* gp_Computer_source_fnames = nullptr;
Base* gp_Computer_bytecode_fnames = nullptr;
Base* gp_Computer_lang_fnames = nullptr;

void learn_dfmt_SourceCode()
{
    using namespace semnet::filesystem::gen;
    using namespace semnet::patterns::gen;

    gp_C_hdr_fname = fnx("h")->set_name("C Header");
    gp_C_src_fname = fnx("c")->set_name("C Source");
    gp_C_fname = alt(gp_C_hdr_fname, gp_C_src_fname)->set_name("C");
    gp_C_ftype = filetype_NaC(gp_C_fname, C::gp_lang, DFMT_C, "C");

    gp_Cxx_hdr_fname = fnx("hpp", "h++", "H", "hxx")->set_name("C++ Header");
    gp_Cxx_src_fname = fnx("cpp", "c++", "C", "cxx")->set_name("C++ Source");
    gp_Cxx_fname = alt(gp_Cxx_hdr_fname, gp_Cxx_src_fname)->set_name("C++ Filename");
    gp_Cxx_ftype = filetype_NaC(gp_Cxx_fname, gp_Cxx_lang, DFMT_C, "C++");

    gp_D_fname = fnx("d")->set_name("D Source");
    gp_D_ftype = filetype_NaC(gp_D_fname, gp_D_lang, DFMT_D, "D");

    gp_Java_src_fname = fnx("java")->set_name("Java Source Code");
    gp_Java_Source_ftype = filetype_NaC(gp_Java_src_fname, nullptr, DFMT_JAVA_SOURCE, "Java");
    gp_Java_Class_src_fname = fnx("class")->set_name("Java Compiled (Class)");
    gp_Java_Class_ftype = filetype_NaC(gp_Java_Class_src_fname, nullptr, DFMT_JAVA_BYTECODE, "Java Bytecode");

    gp_Python_src_fname = fnx("python", "py")->set_name("Python Source");
    gp_Python_ftype = filetype_NaC(gp_Python_src_fname, nullptr, DFMT_PYTHON_SOURCE, "Python");
    gp_Python_Bytecode_fname = fnx("pyc")->set_name("Python Compiled");
    gp_Python_Bytecode_ftype = filetype_NaC(gp_Python_Bytecode_fname, nullptr, DFMT_PYTHON_BYTECODE, "Python Bytecode");

    gp_Elisp_src_fname = fnx("el", "elisp")->set_name("Emacs-Lisp Source");
    gp_Elisp_ftype = filetype_NaC(gp_Elisp_src_fname, nullptr, DFMT_EMACS_LISP_SOURCE, "Emacs-lisp");
    gp_Elisp_Bytecode_fname = fnx("elc")->set_name("Emacs-Lisp Compiled");
    gp_Elisp_Bytecode_ftype = filetype_NaC(gp_Elisp_Bytecode_fname, nullptr, DFMT_EMACS_LISP_BYTECODE, "Emacs-lisp");

    gp_Shell_fname = fnx("sh")->set_name("Shell Script");
    gp_Shell_ftype = filetype_NaC(gp_Shell_fname, nullptr, DFMT_SHELL, "Shell");
    gp_Bash_fname = fnx("bash")->set_name("Bash Script");
    gp_Bash_ftype = filetype_NaC(gp_Bash_fname, nullptr, DFMT_BASH, "Bash");
    gp_Csh_fname = fnx("csh")->set_name("C shell Script");
    gp_Csh_ftype = filetype_NaC(gp_Bash_fname, nullptr, DFMT_BASH, "C shell");

    gp_Makefile_fname = fnx("mk")->set_name("Makefile Source");
    gp_Make_ftype = filetype_NaC(gp_Makefile_fname, nullptr, DFMT_MAKEFILE, "Make Script");
    gp_Makefile_target = seq(bol(),
                             gp_WS_0oM,
                             gp_Letter_EN_us_dig09_1oM,
                             gp_WS_0oM,
                             gp_Colon)->set_name("Makefile Target");

    gp_Julia_fname = fnx("jl")->set_name("Julia Script");
    gp_Matlab_fname = fnx("m")->set_name("Matlab Script");
    gp_Octave_fname = fnx("m")->set_name("Octave Script");

    gp_Computer_source_fnames = alt(gp_C_fname,
                                    gp_Cxx_fname,
                                    gp_D_fname,
                                    gp_Java_src_fname,
                                    gp_Python_src_fname,
                                    gp_Shell_fname,
                                    gp_Makefile_fname,
                                    gp_Julia_fname,
                                    gp_Matlab_fname,
                                    gp_Octave_fname);
    gp_Computer_bytecode_fnames = alt(gp_Java_Class_src_fname,
                                      gp_Python_Bytecode_fname);
    gp_Computer_lang_fnames = alt(gp_Computer_source_fnames,
                                  gp_Computer_bytecode_fnames);
}

Base * gp_ELF_magic = nullptr;

Base * gp_ELF_class = nullptr;
Base * gp_ELF_data = nullptr;
Base * gp_ELF_version = nullptr;
Base * gp_ELF_osabi = nullptr;
Base * gp_ELF_abiversion = nullptr;
Base * gp_ELF_gccversion = nullptr;

Base* gp_ELF_fname = nullptr;
Seq* gp_ELF_dfmt = nullptr;

FileType * gp_ELF_ftype = nullptr;

void learn_dfmt_ELF()
{
    using namespace semnet::filesystem::gen;
    using namespace semnet::patterns::gen;

    gp_ELF_fname = alt(bofn("lib"),
                       fnx("o", "so", "ko"));

    gp_ELF_dfmt = seq();
    gp_ELF_dfmt->set_name("ELF");

    // magic quad
    gp_ELF_dfmt->add_at_BOB(gp_ELF_magic = lit(ELFMAG)->set_name("Magic"), bix(8)); // elf.h: \c ELFMAG
    gp_ELF_dfmt->add_at_BOB(gp_ELF_class = ran(ELFCLASSNONE,3)->set_name("Class"), bix(8*EI_CLASS));
    gp_ELF_dfmt->add_at_BOB(gp_ELF_data = ran(ELFDATANONE,3)->set_name("Data Encoding"), bix(8*EI_DATA));
    gp_ELF_dfmt->add_at_BOB(gp_ELF_version = anybytes(1)->set_name("Version"), bix(8*EI_VERSION));
    gp_ELF_dfmt->add_at_BOB(gp_ELF_osabi = anybytes(1)->set_name("OS ABI Identification"), bix(8*EI_OSABI)); // elf.h defines \c ELFOSABI_NONE
    gp_ELF_dfmt->add_at_BOB(gp_ELF_abiversion = anybytes(1)->set_name("ABI Identification"), bix(8*EI_ABIVERSION)); // elf.h defines \c ELFOSABI_NONE

    // gp_ELF_gccversion = ctx(PCTX_IN_FILE_CONTENTS, seq(gp_NUL, lit_r("GCC: "), gp_Any, gp_NUL))->set_name("ELF GCC Version");

    gp_ELF_ftype = filetype_NoC(gp_ELF_fname, gp_ELF_dfmt, DFMT_ELF, "ELF", "Executable and Linkable File",
                                FileType::PRECOG_CONTENTS);
}

/* ---------------------------- Group Separator ---------------------------- */

Base * gp_tar_posix_magic = nullptr;
Seq * gp_tar_posix_dfmt = nullptr;

Base * gp_Z_dfmt = nullptr;
Base * gp_gz_dfmt = nullptr;
Base * gp_bz_dfmt = nullptr;
Base * gp_xz_dfmt = nullptr;
Base * gp_lzx_dfmt = nullptr;
Base * gp_lzma_dfmt = nullptr;
Base * gp_zip_dfmt = nullptr;
Base * gp_compress_dfmt = nullptr;

void learn_dfmt_compression()
{
    using namespace gen;
    gp_tar_posix_dfmt = seq();
    gp_tar_posix_dfmt->set_name("TAR POSIX");
    // \todo Replace with higher level function.
    gp_tar_posix_dfmt->add_at_BOB(gp_tar_posix_magic = lit("ustar" "\x00" "00", 8), bix(8*257));

    gp_Z_dfmt = lit("\x1f" "\x9d", 2);  // .Z
    gp_gz_dfmt = lit("\x1f" "\x8b", 2);       // ("gz" "gzip" "dz")
    gp_bz_dfmt = lit("BZh", 3);    // ("bzip2" "bz" "bz2" "tbz2")
    gp_xz_dfmt = lit("\xfd" "7xXZ" "\0", 6);       // ("xz" "txz" "7z" "t7z" "lzma" "tlzma" "lz" "tlz")
    gp_lzx_dfmt = lit("LZX", 3);                   // ("lzx")
    gp_lzma_dfmt = lit("LZMA", 4);                   // ("lzx")
    gp_zip_dfmt = lit("\x50" "\x4b" "\x03" "\x04", 4); // ("zip" "zipx")
    gp_compress_dfmt = alt(gp_Z_dfmt,
                           gp_gz_dfmt,
                           gp_bz_dfmt,
                           gp_xz_dfmt,
                           gp_lzx_dfmt,
                           gp_zip_dfmt);
}

/* ---------------------------- Group Separator ---------------------------- */

/*! ID3v1 Tag.
 * \see http://wiki.hydrogenaudio.org/index.php?title=ID3v1
 */
Lit * gp_ID3_tags_magic = nullptr;

Base * gp_ID3v1_tags_magic = nullptr;
Base * gp_ID3v11_tags_magic = nullptr;
Base * gp_ID3v2_tags_magic = nullptr;

Base * gp_ID3_tags = nullptr;

void learn_dfmt_ID3()
{
    using namespace semnet::filesystem::gen;
    using namespace semnet::patterns::gen;

    gp_ID3_tags_magic = lit("ID3");

    gp_ID3v1_tags_magic = seq(bob(), gp_ID3_tags_magic);
    gp_ID3v11_tags_magic = seq(bob(), gp_ID3_tags_magic);
    gp_ID3v2_tags_magic = seq(bob(), gp_ID3_tags_magic);
    gp_ID3_tags = alt(gp_ID3v1_tags_magic,
                      gp_ID3v11_tags_magic,
                      gp_ID3v2_tags_magic);
}

/* ---------------------------- Group Separator ---------------------------- */

Lit * gp_APE_tags_magic = nullptr;
Seq * gp_APE_tags_items = nullptr;

/*!
 * \see https://secure.wikimedia.org/wikipedia/en/wiki/APE_tag
 * \see http://wiki.hydrogenaudio.org/index.php?title=APEv1_specification
 * - At EOF
 * - ASCII in both key names and values.
 */
Base * gp_APEv1_tags = nullptr;

/*!
 * APEv2 Tag.
 *
 * \see https://secure.wikimedia.org/wikipedia/en/wiki/APE_tag
 * \see http://wiki.hydrogenaudio.org/index.php?title=APEv2_specification
 * - At BOF.
 * - ASCII (all by 0x0-0x1f) for key names.
 * - UTF-8 for key values.
 * - Used by WavPack and OptimFROG.
 */
Base * gp_APEv2_tags = nullptr;

void learn_dfmt_APE()
{
    using namespace semnet::patterns::gen;
    gp_APE_tags_magic = lit("APETAGEX"); ///< Magic Header or Footer.
    gp_APE_tags_items = seq();
    gp_APEv1_tags = (seq(gp_APE_tags_items,
                         gp_APE_tags_magic,
                         sit(PRPOS_EOB)))->set_name("APEv1 tag");
    gp_APEv2_tags = (seq(bob(),
                         gp_APE_tags_magic,
                         gp_APE_tags_items,
                         gp_APE_tags_magic))->set_name("APEv2 tag");
}

/* ---------------------------- Group Separator ---------------------------- */

Base * gp_MP3_name = nullptr;
Seq * gp_MP3_dfmt = nullptr;

FileType * gp_MP3_ftype = nullptr;
FileType * gp_WAV_ftype = nullptr;
FileType * gp_AMR_ftype = nullptr;
FileType * gp_MPEG4_ftype = nullptr;

Alt * gp_audio_dfmt = nullptr;

void learn_dfmt_Audio()
{
    using namespace semnet::filesystem::gen;
    using namespace semnet::patterns::gen;

    gp_MP3_dfmt = seq(gp_ID3_tags);
    gp_MP3_name = fnx("mp3");
    gp_MP3_ftype = filetype_NaC(gp_MP3_name, gp_MP3_dfmt, DFMT_AUDIO_MP3,
                                "MP3", "MPEG2 Layer 3");

    auto gp_WAV_name = fnx("wav");
    auto gp_WAV_magic_header = seq(bob(),
                                   lit("RIFF"),
                                   anybytes(4),
                                   lit("WAVEfmt"));
    auto gp_WAV_magic = seq(bob(), gp_WAV_magic_header);
    auto gp_WAV_dfmt = seq(gp_WAV_magic);
    gp_WAV_ftype = filetype_NaC(gp_WAV_name, gp_WAV_dfmt, DFMT_AUDIO_WAV,
                                "WAV", "Waveform Audio File Format");

    auto gp_AMR_name = fnx("amr");
    auto gp_AMR_magic_header = lit("#!AMR" "\x0a", 6);
    auto gp_AMR_magic = seq(bob(), gp_AMR_magic_header);
    auto gp_AMR_dfmt = seq(gp_AMR_magic);
    gp_AMR_ftype = filetype_NaC(gp_AMR_name, gp_AMR_dfmt, DFMT_AUDIO_AMR,
                                "AMR", "Adaptive Multi-Rate audio codec");

    auto gp_MPEG4_name = fnx("m4a");
    auto gp_MPEG4_magic_header = lit("\x00" "\x00" "\x00" "\x20" "\x66" "\x74" "\x79" "\x70"
                                     "\x4d" "\x34" "\x41" "\x20" "\x00" "\x00" "\x00" "\x00",
                                     16);
    auto gp_MPEG4_magic = seq(bob(), gp_MPEG4_magic_header);
    auto gp_MPEG4_dfmt = seq(gp_MPEG4_magic);
    gp_MPEG4_ftype = filetype_NaC(gp_MPEG4_name, gp_MPEG4_dfmt, DFMT_AUDIO_MPEG4,
                                  "MPEG-v4", "MPEG Version 4");

    // TODO: Auto-Generate this from gp_audio_ftypes
    gp_audio_dfmt = alt(gp_MP3_dfmt,
                        gp_WAV_dfmt,
                        gp_AMR_dfmt);
}

void learn_dfmts()
{
    learn_dfmt_SourceCode();
    learn_dfmt_compression();
    learn_dfmt_ELF();
    learn_dfmt_ID3();
    learn_dfmt_APE();
    learn_dfmt_Audio();
}

}
}
