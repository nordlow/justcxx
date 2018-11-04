# GNU Make Configuration and Environment Queryings

# =============================================================================

# System and architecture dependent flags

# Target Operating System.
# Override by, for example, 'make TARGET_OS=Solaris'
TARGET_OS = $(shell uname)

CONFIG_GUESS := $(shell config.guess)

# GCC Installation Prefix
#GCC_PREFIX = /usr
#GCC_PREFIX = $(HOME)/opt/$(CONFIG_GUESS)/gcc-snapshot

# =============================================================================

# Intel x86 Platform
IX86_PLATFORM = $(shell uname -m |sed -n "s/^[xi].*86.*/OK/p")

# x86 is Big-Endian
ifeq ($(IX86_PLATFORM),OK)
MFLAGS_ENDIAN = -DCONFIG_LITTLE_ENDIAN # CPU is Little-Endian
endif

# Generic Processor Type
CPU_TYPE = $(shell uname -p)

# PowerPC is Big-Endian
ifeq ($(CPU_TYPE),powerpc)
MFLAGS_ENDIAN = -DCONFIG_BIG_ENDIAN # CPU is Big-Endian
endif

# =============================================================================

# DVIS Frontends
DVIS_BACKEND_GLUT = 1
DVIS_BACKEND_GLX = 2
DVIS_BACKEND_XLIB = 3
DVIS_BACKEND_AGL = 4			# Apple GL (AGL) on Mac (OS X)

# =============================================================================

# DVIS (Animate Vector Graphics) Default Frontend
DVIS_BACKEND_NAME = glut

ifeq ($(TARGET_OS),Linux)
PNW_USE_MITSHM = true
PNW_USE_GNUC_COMPLEX = true
PNW_USE_JPEG_MMX = false
DVIS_BACKEND_NAME = glx

# =============================================================================
# Machine Specific

# Intel Platform. There probably is a better way of checking tehis!
INTEL_PLATFORM = $(shell cat /proc/cpuinfo |sed -n "s/^vendor_id.*:.*Intel\>.*/OK/p")
AMD_PLATFORM = $(shell cat /proc/cpuinfo |sed -n "s/^vendor_id.*:.*AuthenticAMD\>.*/OK/p")

ifeq ($(INTEL_PLATFORM),OK)
MFLAGS_ENDIAN = -DCONFIG_LITTLE_ENDIAN # override
endif

ifeq ($(AMD_PLATFORM),OK)
MFLAGS_ENDIAN = -DCONFIG_LITTLE_ENDIAN # override
endif

# Pentium 4 Support
PENTIUM4_SUPPORT = $(shell cat /proc/cpuinfo |sed -n "s/^model\ name.*:.*\<Pentium(R) 4\>.*/OK/p")
ifeq ($(PENTIUM4_SUPPORT),OK)
MFLAGS := $(MFLAGS) -march=pentium4
endif

# MMX support
MMX_SUPPORT = $(shell cat /proc/cpuinfo |sed -n "s/^flags.*:.*\<mmx\>.*/OK/p")
ifeq ($(MMX_SUPPORT),OK)
MFLAGS := $(MFLAGS) -mmmx -DHAVE_CPU_MMX
endif

# 3DNow support
3DNOW_SUPPORT = $(shell cat /proc/cpuinfo |sed -n "s/^flags.*:.*\<3dnow\>.*/OK/p")
ifeq ($(3DNOW_SUPPORT),OK)
MFLAGS := $(MFLAGS) -m3dnow -DHAVE_CPU_3DNOW
endif

# 3DNowExt support
3DNOWEXT_SUPPORT = $(shell cat /proc/cpuinfo |sed -n "s/^flags.*:.*\<3dnowext\>.*/OK/p")
ifeq ($(3DNOWEXT_SUPPORT),OK)
MFLAGS := $(MFLAGS) -m3dnow -DHAVE_CPU_3DNOWEXT
endif

# SSE support
SSE_SUPPORT = $(shell cat /proc/cpuinfo |sed -n "s/^flags.*:.*\<sse\>.*/OK/p")
ifeq ($(SSE_SUPPORT),OK)
MFLAGS := $(MFLAGS) -msse -DHAVE_CPU_SSE
#MFLAGS := $(MFLAGS) -mfpmath=sse
endif

# SSE2 support
SSE2_SUPPORT = $(shell cat /proc/cpuinfo |sed -n "s/^flags.*:.*\<sse2\>.*/OK/p")
ifeq ($(SSE2_SUPPORT),OK)
MFLAGS := $(MFLAGS) -msse2 -DHAVE_CPU_SSE2
endif

# SSE3 support
SSE3_SUPPORT = $(shell cat /proc/cpuinfo |sed -n "s/^flags.*:.*\<sse3\>.*/OK/p")
ifeq ($(SSE3_SUPPORT),OK)
MFLAGS := $(MFLAGS) -msse3 -DHAVE_CPU_SSE3
endif

# Use native optimizations
MFLAGS := $(MFLAGS) -march=native

# =============================================================================
# Large file Support (LFS)

# See: http://www.suse.de/~aj/linux_lfs.html

# For using LFS in user programs, the programs have to use the LFS API. This
# involves recompilation and changes of programs. The API is documented in the
# glibc manual (the libc info pages) which can be read with e.g. "info libc".

# In a nutshell for using LFS you can choose either of the following:

# - Compile your programs with "gcc -D_FILE_OFFSET_BITS=64". This forces all file
#   access calls to use the 64 bit variants. Several types change also, e.g. off_t
#   becomes off64_t. It's therefore important to always use the correct types and
#   to not use e.g. int instead of off_t. For portability with other platforms you
#   should use getconf LFS_CFLAGS which will return -D_FILE_OFFSET_BITS=64 on
#   Linux platforms but might return something else on e.g. Solaris. For linking,
#   you should use the link flags that are reported via getconf LFS_LDFLAGS. On
#   Linux systems, you do not need special link flags.
# - Define _LARGEFILE_SOURCE and _LARGEFILE64_SOURCE. With these defines you can use the LFS functions like open64 directly.
# - Use the O_LARGEFILE flag with open to operate on large files.

# A complete documentation of the feature test macros like _FILE_OFFSET_BITS and
# _LARGEFILE_SOURCE is in the glibc manual (run e.g. "info libc 'Feature Test
# Macros'").

# The LFS API is also documented in the LFS standard which is available at
# http://ftp.sas.com/standards/large.file/x_open.20Mar96.html.  LFS and
# Libraries other than Glibc

# Be careful when using _FILE_OFFSET_BITS=64 to compile a program that calls a
# library or a library if any of the interfaces uses off_t. With
# _FILE_OFFSET_BITS=64 glibc will change the type of off_t to off64_t. You can
# either change the interface to always use off64_t, use a different function if
# _FILE_OFFSET_BITS=64 is used (like glibc does). Otherwise take care that both
# library and program have the same _FILE_OFFSET_BITS setting. Note that glibc
# is aware of the _FILE_OFFSET_BITS setting, there's no problem with it but
# there might be problems with other libraries.

MFLAGS := $(MFLAGS) -D_FILE_OFFSET_BITS=64

# =============================================================================

OS_FLAGS = #-D__LINUX__ -D__BYTESWAP__
# -I. needed for my boost stuff
SYS_INCLUDE_PATH = -I. -Ideps -Iboost-extras -Iboost-process
LIB_PATH = -L$(HOME)/alt/lib -Ldeps -L/usr/X11R6/lib -L/usr/lib -L/lib
X11_LIBS = -lX11 -lXt -lXext #-lXm
#IMG_LIBS = -lgif -ltiff -lpng
LIBS := $(LIBS) -lm -lc -lpthread

# LLVM
LLVM_BUILD_TYPE=Release
LLVM_SRC_PATH = $(HOME)/ware/llvm #Unify with pbuild settings
LLVM_BUILD_PATH = $(HOME)/ware/llvm-build
LLVM_BIN_PATH = $(HOME)/alt/bin
LLVM_LIBS_LIST = core mc
LLVM_CXXFLAGS = $(shell $(LLVM_BIN_PATH)/llvm-config --cxxflags)
LLVM_LDFLAGS = $(shell $(LLVM_BIN_PATH)/llvm-config --ldflags)
LLVM_LIBS = $(LLVM_BIN_PATH)/llvm-config --libs $(LLVM_LIBS_LIST)
LLVM_CONFIG_COMMAND = $(LLVM_CXXFLAGS) $(LLVM_LDFLAGS) $(LLVM_LIBS)

# Clang
CLANG_BUILD_FLAGS = -I$(LLVM_BUILD_PATH)/tools/clang/include
#CLANG_BUILD_FLAGS = -I$(LLVM_SRC_PATH)/tools/clang/include -I$(LLVM_BUILD_PATH)/tools/clang/include
CLANG_LIBS = \
  -lclangFrontendTool -lclangFrontend -lclangDriver \
  -lclangSerialization -lclangCodeGen -lclangParse \
  -lclangSema -lclangStaticAnalyzerFrontend \
  -lclangStaticAnalyzerCheckers -lclangStaticAnalyzerCore \
  -lclangAnalysis -lclangARCMigrate -lclangRewrite \
  -lclangEdit -lclangAST -lclangLex -lclangBasic

endif

# =============================================================================

ifeq ($(TARGET_OS),SunOS)

PNW_USE_MITSHM = true
PNW_USE_GNUC_COMPLEX = false
DVIS_BACKEND_NAME = x11

# big endian
MFLAGS_ENDIAN = -DCONFIG_BIG_ENDIAN

OS_FLAGS = -D__SOLARIS__
SYS_INCLUDE_PATH = -I/usr/openwin/include -I/opt/sfw/include
LIB_PATH = -L. -L/usr/openwin/lib -L/lib -I/opt/sfw/lib
X11_LIBS = -lX11 -lXt -lXext#-lXm
#IMG_LIBS = -lgif -ljpeg -ltiff -lpng
LIBS := $(LIBS) -lposix4 -lsocket -lnsl -lm -lc -lpthread

endif

# =============================================================================

ifeq ($(TARGET_OS),Darwin)

GCC_PREFIX = /usr

PNW_USE_MITSHM = false
PNW_USE_GNUC_COMPLEX = false
DVIS_BACKEND_NAME = glut

# big endian
MFLAGS_ENDIAN = -DCONFIG_BIG_ENDIAN
MFLAGS = -DGL_EXT_vertex_array

OS_FLAGS =
SYS_INCLUDE_PATH = -I$(HOME)/alt/include -I/usr/X11R6/include
LIB_PATH = -L$(HOME)/alt/lib -L/usr/X11R6/lib
X11_LIBS = -lX11 -lXt -lXext #-lXm
#IMG_LIBS = -lgif -ljpeg -ltiff -lpng
LIBS := $(LIBS) -lm -lmx -lc -lpthread
endif

# =============================================================================

# Compiler names

# ccache supports C,C++,Objective-C and Objective-C++
# ./configure CC='ccache clang -Qunused-arguments -fcolor-diagnostics'

#GCC_VERSION_SUFFIX=-4.7
CCACHE=ccache
CC = $(CCACHE) gcc${GCC_VERSION_SUFFIX}
CXX = $(CCACHE) g++${GCC_VERSION_SUFFIX}
FC = gfortran-${GCC_VERSION_SUFFIX}
# CC = clang -x c
# CXX = clang -x c++

LD = ld.gold
# Use GNU Extensions.
# Whats the difference between \c _GNU_SOURCE and \c __USE_GNU?
DEF_FLAGS := $(DEF_FLAGS) -D_GNU_SOURCE -D__USE_GNU

# =============================================================================

# Thread Support
# Use threaded version of functions from system and libraries.
DEF_FLAGS := $(DEF_FLAGS) -DPREEMPT

# GMP: libgmp-dev
DEF_FLAGS := $(DEF_FLAGS) -DPNW_USE_LIBGMP -DHAVE_GMP_H
GMP_LIBS := -lgmp

# Nettle: libnettle4
NETTLE_CFLAGS := $(shell pkg-config --cflags nettle 2> /dev/null)
NETTLE_LIBS := $(shell pkg-config --libs nettle 2> /dev/null)

# OpenCV: libopencv-dev
# OPENCV_CFLAGS := $(shell pkg-config --cflags opencv 2> /dev/null)
# OPENCV_LIBS := $(shell pkg-config --libs opencv 2> /dev/null)
# OPENCV_GPU_LIBS := -lopencv_gpu

# UDUNITS2: libudunits2-dev
DEF_FLAGS := $(DEF_FLAGS) -DHAVE_UDUNITS2_H
MAGIC_LIBS := -ludunits2

# libmagic: libmagic-dev
DEF_FLAGS := $(DEF_FLAGS) -DHAVE_MAGIC_H
MAGIC_LIBS := -lmagic

# ZeroMQ (ZMQ): libzmq-dev
ZMQ_CFLAGS := $(shell pkg-config --cflags libzmq 2> /dev/null)
ZMQ_LIBS := $(shell pkg-config --libs libzmq 2> /dev/null)

# GLFW
GLFW_CFLAGS := $(shell pkg-config --cflags libglfw 2> /dev/null)
GLFW_LIBS := $(shell pkg-config --libs libglfw 2> /dev/null)

# GLFW3
GLFW3_CFLAGS := $(shell pkg-config --cflags glfw3 2> /dev/null)
GLFW3_LIBS := $(shell pkg-config --libs glfw3 2> /dev/null)

# libxml: libxml2-dev
XML_CFLAGS := $(shell pkg-config --cflags libxml-2.0 2> /dev/null)
XML_LIBS := $(shell pkg-config --libs libxml-2.0 2> /dev/null)

WX_CFLAGS := $(shell wx-config --cflags 2> /dev/null)
WX_LIBS := $(shell wx-config --libs 2> /dev/null)

GLUT_CFLAGS := -I/usr/include/GL
GLUT_LIBS := -lglut

GLEW_CFLAGS := $(shell pkg-config --cflags glew 2> /dev/null)
GLEW_LIBS := $(shell pkg-config --libs glew 2> /dev/null)

IGRAPH_CFLAGS := $(shell pkg-config --cflags igraph 2> /dev/null)
IGRAPH_LIBS := $(shell pkg-config --libs igraph 2> /dev/null)

#OGLPLUS_CFLAGS := $(HOME)/opt/x86_64-unknown-linux-gnu/oglplus-0.24.0/include
#OGLPLUS_CFLAGS := $(HOME)/opt/x86_64-unknown-linux-gnu/oglplus-0.25.0/include
OGLPLUS_CFLAGS := $(HOME)/opt/x86_64-unknown-linux-gnu/oglplus-snapshot/include

# libcurl: libcurl4-openssl-dev
CURL_LIBS := -lcurl

# mitshm - MIT Shared Memory Extension for XImages
ifeq ($(PNW_USE_MITSHM),true)
    DEF_FLAGS := $(DEF_FLAGS) -DPNW_USE_MITSHM
endif

# readline: libreadline-dev
ifeq ($(TARGET_OS),Linux)
    READLINE_LIBS := -lreadline -lncurses
endif
ifeq ($(TARGET_OS),SunOS)
    READLINE_LIBS := -lreadline -lhistory -ltermcap
endif
ifeq ($(TARGET_OS),Darwin)
    READLINE_LIBS := -lreadline -lncurses
endif

# JPEG or JPEG-mmx: libjpeg-turbo62 libjpeg8-dev
ifeq ($(PNW_USE_JPEG_MMX),true)
    JPEG_SYS_INCLUDE_PATH := $(SYS_INCLUDE_PATH) -I/usr/include/jpeg-mmx
    JPEG_LIBS := -ljpeg-mmx
else
    JPEG_LIBS := -ljpeg
endif
IMG_LIBS := $(JPEG_LIBS)

# gnuc_complex
ifeq ($(PNW_USE_GNUC_COMPLEX),true)
    DEF_FLAGS := $(DEF_FLAGS) -DPNW_USE_GNUC_COMPLEX
endif

# =============================================================================

# DVIS Settings

# Override if x11 was defined "externally".
ifeq ($(DVIS_BACKEND_NAME),x11)
DVIS_BACKEND = $(DVIS_BACKEND_XLIB)
endif

# Override if glut was defined "externally".
ifeq ($(DVIS_BACKEND_NAME),glut)
DVIS_BACKEND = $(DVIS_BACKEND_GLUT)
endif

# Override if glx was defined "externally".
ifeq ($(DVIS_BACKEND_NAME),glx)
DVIS_BACKEND = $(DVIS_BACKEND_GLX)
endif

# Override if agl was defined "externally".
ifeq ($(DVIS_BACKEND_NAME),agl)
DVIS_BACKEND = $(DVIS_BACKEND_AGL)
endif

DEF_FLAGS := $(DEF_FLAGS) -DDVIS_BACKEND=$(DVIS_BACKEND)
DEF_FLAGS := $(DEF_FLAGS) \
	-DDVIS_BACKEND_GLUT=$(DVIS_BACKEND_GLUT) \
	-DDVIS_BACKEND_GLX=$(DVIS_BACKEND_GLX) \
	-DDVIS_BACKEND_XLIB=$(DVIS_BACKEND_XLIB) \
	-DDVIS_BACKEND_AGL=$(DVIS_BACKEND_AGL)

# =============================================================================

# GLUT Frameworks. grep for dylib in DVIS_GL.xcodeproj
ifeq ($(DVIS_BACKEND),$(DVIS_BACKEND_GLUT))
ifeq ($(TARGET_OS),Darwin)
GL_CFLAGS := -I/System/Library/Frameworks/OpenGL.framework/Headers -I/System/Library/Frameworks/GLUT.framework/Headers $(SYS_INCLUDE_PATH)
GL_LIBS := -L/System/Library/Frameworks/OpenGL.framework/Versions/Current/Libraries $(LIB_PATH) -lGL -lGLU /System/Library/Frameworks/GLUT.framework/Versions/Current/GLUT
else
GL_LIBS := -lGL -lGLU -lglut
endif
endif

# Apple GL
# AGL Framework Reference (http://developer.apple.com/documentation/GraphicsImaging/Reference/AGL_OpenGL/Reference/reference.html)
ifeq ($(DVIS_BACKEND),$(DVIS_BACKEND_AGL))
GL_CFLAGS := -I/System/Library/Frameworks/OpenGL.framework/Headers -I/System/Library/Frameworks/AGL.framework/Headers $(SYS_INCLUDE_PATH)
GL_LIBS := -L/System/Library/Frameworks/OpenGL.framework/Versions/Current/Libraries $(LIB_PATH) -lGL -lGLU /System/Library/Frameworks/AGL.framework/Versions/Current/AGL
endif

# GLX
ifeq ($(DVIS_BACKEND),$(DVIS_BACKEND_GLX))
    GL_CFLAGS := -I/usr/include/GL
    GL_LIBS := -lGL -lGLU
endif

ifeq ($(DVIS_BACKEND),$(DVIS_BACKEND_XLIB))
endif

# =============================================================================

# GTK
GTK_CFLAGS := $(shell pkg-config --cflags gtk+-3.0 2> /dev/null)
GTK_LIBS := $(shell pkg-config --libs gtk+-3.0 2> /dev/null)

# GTKMM
GTKMM_CFLAGS := $(shell pkg-config --cflags gtkmm-3.0 2> /dev/null)
GTKMM_LIBS := $(shell pkg-config --libs gtkmm-3.0 2> /dev/null)

# GStreamer
GST_CFLAGS := $(shell pkg-config --cflags gstreamer-0.10 2> /dev/null)
GST_LIBS := $(shell pkg-config --libs gstreamer-0.10 2> /dev/null)

# Freetype2
# /sw/lib/freetype219/bin/freetype-config
DEF_FLAGS := $(DEF_FLAGS)
FREETYPE_CFLAGS := $(shell freetype-config --cflags 2> /dev/null)
FREETYPE_LIBS := $(LIBS) $(shell freetype-config --libs 2> /dev/null)

# Liboil
LIBOIL_CFLAGS := $(shell pkg-config --cflags liboil-0.3 2> /dev/null)
LIBOIL_LIBS := $(shell pkg-config --libs liboil-0.3 2> /dev/null)

# LLVM (Low Level Virtual Machine)
LLVM_CFLAGS := $(shell if which llvm-config; then llvm-config --cflags; fi)
LLVM_LIBS := $(shell if which llvm-config; then llvm-config --libs; fi)

# google-sparsehash (http://code.google.com/p/google-sparsehash/)
SPARSEHASH_CFLAGS := $(shell pkg-config --cflags libsparsehash 2> /dev/null)
SPARSEHASH_LIBS := $(shell pkg-config --libs libsparsehash 2> /dev/null)

# Eigen
EIGEN3_CFLAGS := $(shell pkg-config --cflags eigen3 2> /dev/null)
EIGEN3_LIBS := $(shell pkg-config --libs eigen3 2> /dev/null)

# Armadillo
ARMADILLO_VERSION = 3.800.1
ARMADILLO_CFLAGS := -I$(HOME)/opt/$(CONFIG_GUESS)/armadillo-$(ARMADILLO_VERSION)/include
ARMADILLO_LIBS := -L$(HOME)/opt/$(CONFIG_GUESS)/armadillo-$(ARMADILLO_VERSION)/lib -larmadillo

BOOST_VERSION := 1.53
BOOST_OPT_CFLAGS := -I$(HOME)/opt/$(CONFIG_GUESS)/boost-$(BOOST_VERSION)/include
BOOST_OPT_LIBS := -L$(HOME)/opt/$(CONFIG_GUESS)/boost-$(BOOST_VERSION)/lib

# =============================================================================

# See: http://stackoverflow.com/questions/3375697/useful-gcc-flags-for-c

SYS_INCLUDE_PATH := $(SYS_INCLUDE_PATH)
LIB_PATH := $(LIB_PATH) -L$(GCC_PREFIX)/lib

LDFLAGS = $(LIB_PATH) $(LIBS)

# Warnings used in both C and C++
# -Wold-style-cast: Warn about old-style C cast
# -Wextra, -Wall: essential.
# -Wfloat-equal: useful because usually testing floating-point numbers for equality is bad.
# -Wundef: warn if an uninitialized identifier is evaluated in an #if directive.
# -Wshadow: warn whenever a local variable shadows another local variable, parameter or global variable or whenever a built-in function is shadowed.
# -Wpointer-arith: warn if anything depends upon the size of a function or of void.
# -Wcast-align: warn whenever a pointer is cast such that the required alignment of the target is increased. For example, warn if a char * is cast to an int * on machines where integers can only be accessed at two- or four-byte boundaries.
# -Wstrict-prototypes: warn if a function is declared or defined without specifying the argument types.
# -Wstrict-overflow=5: warns about cases where the compiler optimizes based on the assumption that signed overflow does not occur. (The value 5 may be too strict, see the manual page.)
# -Wwrite-strings: give string constants the type const char[length] so that copying the address of one into a non-const char * pointer will get a warning.
# -Waggregate-return: warn if any functions that return structures or unions are defined or called.
# -Wcast-qual: warn whenever a pointer is cast to remove a type qualifier from the target type*.
# -Wswitch-default: warn whenever a switch statement does not have a default case*.
# -Wswitch-enum: warn whenever a switch statement has an index of enumerated type and lacks a case for one or more of the named codes of that enumeration*.
# -Wconversion: warn for implicit conversions that may alter a value*.
# -Wno-sign-conversion:
# -Wunreachable-code: warn if the compiler detects that code will never be executed*.
# -ftrapv: function will cause the program to abort on signed integer overflow (formally "undefined behaviour" in C).
# -fverbose-asm: is useful if you're compiling with -S to examine the assembly output - it adds some informative comments.
# -Winit-self is useful; I wonder why it's not included in -Wuninitialized.
WARN_FLAGS = -Wall -Wchar-subscripts -Wpointer-arith -Wcast-align -Wsign-compare -Wmissing-braces -Wwrite-strings -Winit-self -Wunused-but-set-variable -Wempty-body -Wunused-but-set-parameter -Wunreachable-code #-Wfatal-errors  #-Wunused-parameter

# Clang 3.1 http://clang.llvm.org/docs/ReleaseNotes.html
CLANG_WARN_FLAGS = -Wdangling-else -Wstrncat-size -Wliteral-conversion

# Extra warnings only used in C
C_WARN_FLAGS = -Wstrict-prototypes
#CXX_WARN_FLAGS = -Woverloaded-virtual #-Weffc++

# Compatibility Standards
COMPAT_FLAGS = -D_REENTRANT -D__STDC_CONSTANT_MACROS # FFMPEG requires __STDC_CONSTANT_MACROS to be defined if we use it in C++
C_COMPAT_FLAGS = $(COMPAT_FLAGS) -std=gnu99
CXX_COMPAT_FLAGS = $(COMPAT_FLAGS) -std=gnu++0x

# =============================================================================
# Machine specific flags

# either use -g or -ggdb
DEBUG_FLAGS = -ggdb3 -DDEBUG -DDEBUG_CHECK_ALL -fno-inline

# TODO: Add Option: `Pointer-Arithmetic-Error-Detect' `-fmudflap'
# -fmudflap -- adds runtime checks to all risky pointer operations to catch UB.

# TODO: Add Option: `Stack-Overflow-Protect' `-fstack-check'
# TODO: Add Option: `Integer-Arithmetic-Overflow-Detect' `-gnato'

# Always use -O or above (-O1, -O2, -Os, etc.). At the default optimization level, gcc goes for compilation speed and doesn't do enough analysis to warn about things like unitialized variables.
# See: http://gcc.gnu.org/wiki/Mudflap_Pointer_Debugging

# Default build type
BUILD_TYPE=debug

#    BUILD_TYPE_FLAGS = $(DEBUG_FLAGS) -fsanitize=address -fno-omit-frame-pointer
ifeq ($(BUILD_TYPE),debug)
    BUILD_TYPE_FLAGS = $(DEBUG_FLAGS)
endif

ifeq ($(BUILD_TYPE),malloc-debug)
    ifeq ($(TARGET_OS),Darwin)
				DEBUG_FLAGS := $(DEBUG_FLAGS) -lMallocDebug
    endif
    BUILD_TYPE_FLAGS = $(DEBUG_FLAGS) -fno-inline
endif
ifeq ($(BUILD_TYPE),small-debug)
    BUILD_TYPE_FLAGS = -O2 $(DEBUG_FLAGS)
endif
ifeq ($(BUILD_TYPE),opt)
    BUILD_TYPE_FLAGS = -O -DNDEBUG -DARMA_NO_DEBUG
    LDFLAGS := $(LDFLAGS) -s
endif
ifeq ($(BUILD_TYPE),small)
    BUILD_TYPE_FLAGS = -O2 -DNDEBUG -DARMA_NO_DEBUG
    LDFLAGS := $(LDFLAGS) -s
endif

ifeq ($(BUILD_TYPE),fast)
    BUILD_TYPE_FLAGS = -O3 -funroll-loops -fomit-frame-pointer -ftree-vectorize -DNDEBUG -DARMA_NO_DEBUG
    LDFLAGS := $(LDFLAGS) -s
endif
ifeq ($(BUILD_TYPE),fast-debug)
    BUILD_TYPE_FLAGS = -O3 -funroll-loops $(DEBUG_FLAGS)
endif

ifeq ($(BUILD_TYPE),release)
    WARN_FLAGS := $(WARN_FLAGS) -Wunused-parameter # In releases everything should be implemented and used.
    BUILD_TYPE_FLAGS = -O6 -funroll-loops -fomit-frame-pointer -ftree-vectorize -fwhole-program -flto -DNDEBUG -DARMA_NO_DEBUG
    LDFLAGS := $(LDFLAGS) -s -flto
endif
ifeq ($(BUILD_TYPE),parallel-release)
    WARN_FLAGS := $(WARN_FLAGS) -Wunused-parameter # In releases everything should be implemented and used.
    BUILD_TYPE_FLAGS = -D_GLIBCXX_PARALLEL -fopenmp -O6 -funroll-loops -fomit-frame-pointer -ftree-vectorize -fwhole-program -flto -DNDEBUG -DARMA_NO_DEBUG
    LDFLAGS := $(LDFLAGS) -s -flto
endif
ifeq ($(BUILD_TYPE),release-verbose)
    WARN_FLAGS := $(WARN_FLAGS) -Wunused-parameter # In releases everything should be implemented and used.
    BUILD_TYPE_FLAGS = -O6 -funroll-loops -fomit-frame-pointer -ftree-vectorize -ftree-vectorizer-verbose=5 -fwhole-program -flto -DNDEBUG -DARMA_NO_DEBUG
    LDFLAGS := $(LDFLAGS) -s
endif
ifeq ($(BUILD_TYPE),profile)
    BUILD_TYPE_FLAGS = -pg $(DEBUG_FLAGS)
endif
# adds code to call user-supplied profiling functions at every function entry and exit point.
ifeq ($(BUILD_TYPE),instrumented)
    BUILD_TYPE_FLAGS = -finstrument-functions $(DEBUG_FLAGS)
endif
ifeq ($(BUILD_TYPE),opt-profile)
    BUILD_TYPE_FLAGS = -O -pg $(DEBUG_FLAGS)
endif
ifeq ($(BUILD_TYPE),small-profile)
    BUILD_TYPE_FLAGS = -O2 -pg $(DEBUG_FLAGS)
endif
ifeq ($(BUILD_TYPE),fast-profile)
    BUILD_TYPE_FLAGS = -O3 -funroll-loops -ftree-vectorize -pg $(DEBUG_FLAGS) -DNDEBUG -DARMA_NO_DEBUG
endif
ifeq ($(BUILD_TYPE),intel)
    CXX = icc
    BUILD_TYPE_FLAGS = -O3 -xK -tpp6 -ip -openmp
endif

# TODO Add "-DARMA_NO_DEBUG" if $(BUILD_TYPE_FLAGS) contains "-DNDEBUG"

# =============================================================================

# @TODO: Precompiled Headers (PCH)
#
# - http://wiki.wxwidgets.org/Precompiled_Headers
# - http://tombarta.wordpress.com/2009/12/12/precompiled-headers-with-gcc/
# - http://www.mercs-eng.com/~hulud/index.php?2008/06/13/6-writing-a-good-makefile-for-a-c-project
# - http://gcc.gnu.org/ml/gcc-help/2008-04/msg00245.html
# - https://bugs.eclipse.org/bugs/show_bug.cgi?id=89155
#
# - Note: GCC uses the extension .h.gch
# - Note: Clang's' PCH is about 10 times smaller than GCC's!
# - Note: Google: precompiled header makefile gcc

# @TODO: Issue warning if BUILD_TYPE is defined but does not fit in any cases above.

# =============================================================================

# Compilation flags common to C ad C++.
CFLAGS_COMMON = $(DEF_FLAGS) $(WARN_FLAGS) \
		$(BUILD_TYPE_FLAGS) \
		$(MFLAGS) $(OS_FLAGS) $(SYS_INCLUDE_PATH) \
		$(FREETYPE_CFLAGS) $(GTK_CFLAGS) $(GTKMM_CFLAGS) $(NETTLE_CFLAGS) $(ARMADILLO_CFLAGS) \
		-DHAVE_CONFIG_H -Wshadow #-Wuninitialized #-Wextra #-fshow-column

# Put it all together
CFLAGS = $(CFLAGS_COMMON) $(C_WARN_FLAGS) $(C_COMPAT_FLAGS)
CXXFLAGS = $(CFLAGS_COMMON) $(CXX_WARN_FLAGS) $(CXX_COMPAT_FLAGS)

CCMAIN = $(CC) $(CFLAGS) $(LDFLAGS)
CXXMAIN = $(CXX) $(CXXFLAGS) $(LDFLAGS)

# Auto-Generates Dependency files using -MD.
# See: http://gcc.gnu.org/onlinedocs/gcc/Preprocessor-Options.html
CC_VERB = CC
CXX_VERB = CXX
LD_VERB = LD
AR_VERB = AR
CCOBJ = $(CC) $(CFLAGS) -c -MD -MF $(dir $<).deps/$(notdir $<).dep $< -o $@
CXXOBJ = $(CXX) $(CXXFLAGS) -c -MD -MF $(dir $<).deps/$(notdir $<).dep $< -o $@
CLEAN_VERB = CLEAN

.PHONY: GNUmakefile conf.mk

# C Object
.c.o: GNUmakefile conf.mk
	@mkdir -p $(dir $<).deps # assure dependency directory
ifeq ($(V),1)			# verbose output flag
	$(CCOBJ)
else
	@echo "*** $(CC_VERB)-$(BUILD_TYPE)" $< to $@
	@$(CCOBJ)
endif

# C++ Object
.cpp.o: GNUmakefile conf.mk
	@mkdir -p $(dir $<).deps # assure dependency directory
ifeq ($(V),1)			# verbose output flag
	$(CXXOBJ)
else
	@echo "*** $(CXX_VERB)-$(BUILD_TYPE)" $< to $@
	@$(CXXOBJ)
endif

# Include Dependecy Files in current and sub-directories
C_DEPS = $(wildcard .deps/*.dep) $(wildcard */.deps/*.dep)
include $(C_DEPS)

# Export them (to subdirectories)
export CC CXX CFLAGS CXXFLAGS SYS_INCLUDE_PATH LIB_PATH CCMAIN CCOBJ CXXMAIN CXXOBJ C_DEPS
                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                               
