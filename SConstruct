#!/usr/bin/env scons-file
# \see http://stackoverflow.com/questions/6194900/how-to-work-around-scons-warning-two-different-environments-were-specified-fo
# \see http://www.electric-cloud.com/blog/2010/08/11/the-last-word-on-scons-performance/

# TODO: Support oclint: http://docs.oclint.org/en/dev/intro/tutorial.html
# TODO: Add D Support Configuration: http://stackoverflow.com/questions/10428656/in-sconstruct-how-can-i-specify-include-paths-for-d-programming-language
# TODO: Use os.walk(top, topdown=True, onerror=None, followlinks=False)

# TODO:
# vars = Variables()
# vars.Add(EnumVariable('PLATFORM', 'CPU type', 'i386',
#                       allowed_values = ('i386', 'powerpc',
#                                         'alpha')))
# env = Environment(variables = vars, CFLAGS='-g',
#                   CC='/tools/bin/gcc-${PLATFORM}')
# Export('env')
# Help(vars.GenerateHelpText(env))
# platform = env['PLATFORM']
# SConscript('src/SConscript',
#            variant_dir='obj/%s' % platform)

import os, os.path, glob, sys, re, subprocess

def scons_symlink(target, source, env):
    """Create Symbolic Link."""
    assert(len(target) == 1 and
           len(source) == 1)
    # TODO: How to handle absolute or relative paths?
    os.symlink(os.path.abspath(str(source[0])),
               os.path.abspath(str(target[0])))

def scons_hardlink(target, source, env):
    """Create (Hard) Link."""
    assert(len(target) == 1 and
           len(source) == 1)
    os.link(os.path.abspath(str(source[0])),
            os.path.abspath(str(target[0])))

# TODO: Optionize these to GUI
useClang = False                 # Flag to use the Clang Compiler instead of GCC
useNative = True                # Flag to use Native Code Generation when possible
useGoldLinker = True            # Use ld.gold instead of ld.bfd
useColors = False            # Colorize Compilation Output (GCC 4.9)
useTime = False                  # Time all Compilations
STD_C = 'gnu99'                 # C Standard: gnu99, c99
STD_CXX = 'gnu++11'             # C++ Standard: gnu++11, gnu++1y

HOME = os.environ['HOME']       # home directory

# C Standard
STD_CFLAGS = [ '-std=' + STD_C ]
# C++ Standard
STD_CXXFLAGS = [ '-std=' + STD_CXX ]

# Pick Compilers
if useClang:
    # TODO: Activate flag '-fcolor-diagnostics' only when called from a real terminal
    CC = 'clang'.split()
    CXX = 'clang++'.split()
    STD_CXXFLAGS += ['-D__STRICT_ANSI__'] # http://stackoverflow.com/questions/13525774/clang-and-float128-bug-error
else:
    CC = 'gcc'.split()
    CXX = 'g++'.split()

if useColors:
    CC.append('-fdiagnostics-color=auto')
    CXX.append('-fdiagnostics-color=auto')

if useGoldLinker:
    CC.append('-fuse-ld=gold')
    CXX.append('-fuse-ld=gold')

if useNative:
    STD_CFLAGS += ['-march=native']
    STD_CXXFLAGS += ['-march=native']

if useTime:
    CC.insert(0, 'time')
    CXX.insert(0, 'time')

# Debug
RELEASE_OPT_FLAGS = str.split('-O3 -funroll-loops -fomit-frame-pointer -ftree-vectorize -fwhole-program -flto')
DEBUG_FLAGS = str.split('-g3 -ggdb3')

# C/C++ Common Warnings
WARN_FLAGS = str.split('-Wall -Wshadow -Wchar-subscripts -Wpointer-arith -Wcast-align -Wsign-compare -Wmissing-braces -Wwrite-strings -Werror-implicit-function-declaration')
if useClang:
    WARN_FLAGS += [ '-Wdangling-else', # Version 3.1
                    '-Wstrncat-size', # Version 3.1
                    '-Wsometimes-uninitialized', # Version 3.2
                    '-Wdocumentation']           # Version 3.2

# C Warnings
WARN_CFLAGS = str.split('-Wstrict-prototypes') #-Wunused-parameter
# C++ Warnings
WARN_CXXFLAGS = str.split('-Wparentheses -Wpointer-arith')

# TODO: Pick recent variant of wildcards for _INCLUDE and _LIBPATH and notify user

# Boost Process
BOOST_PROCESS_INCLUDE = 'deps/boost-process'
# Boost Endian
BOOST_ENDIAN_INCLUDE = 'deps/boost-endian/include'

# Nettle
NETTLE_INCLUDE = Glob(os.path.join(HOME, 'opt/x86_64-unknown-linux-gnu/nettle-*/include'))
NETTLE_LIBPATH = glob.glob(os.path.join(HOME, 'opt/x86_64-unknown-linux-gnu/nettle-*/lib'))

# Armadillo
ARMA_INCLUDE = Glob(os.path.join(HOME, 'opt/x86_64-unknown-linux-gnu/armadillo-*/include'))
ARMA_LIBPATH = Glob(os.path.join(HOME, 'opt/x86_64-unknown-linux-gnu/armadillo-*/lib'))

# GLFW3
GLFW3_INCLUDE = Glob(os.path.join(HOME, 'opt/x86_64-unknown-linux-gnu/glfw-3.0.1/include'))
GLFW3_LIBPATH = Glob(os.path.join(HOME, 'opt/x86_64-unknown-linux-gnu/glfw-3.0.1/lib'))

# oglplus
OGLPLUS_INCLUDE = Glob(os.path.join(HOME, 'opt/x86_64-unknown-linux-gnu/oglplus-*/include'))
OGLPLUS_LIBPATH = Glob(os.path.join(HOME, 'opt/x86_64-unknown-linux-gnu/oglplus-*/lib'))

# D Libraryes
D_LIBPATH = Glob(os.path.join(HOME, 'opt/x86_64-unknown-linux-gnu/dmd/lib'))

# Boost
BOOST_INCLUDE = Glob(os.path.join(HOME, 'opt/x86_64-unknown-linux-gnu/boost-*/include'))
BOOST_INCLUDE_S = glob.glob(os.path.join(HOME, 'opt/x86_64-unknown-linux-gnu/boost-*/include'))
BOOST_LIBPATH = Glob(os.path.join(HOME, 'opt/x86_64-unknown-linux-gnu/boost-*/lib'))
BOOST_INCLUDE = []
BOOST_LIBPATH = []

# Fix This
CONFIG_ENDIAN = '-DCONFIG_LITTLE_ENDIAN'
CONFIG_OTHER = []

# semnet
SEMNET_ALL = glob.glob('semnet/*.cpp')
SEMNET_MAIN = 'semnet/main.cpp'
SEMNET_SRC = [f for f in SEMNET_ALL if f != SEMNET_MAIN] # all except main program

# strace
STRACE_ALL = glob.glob('strace/*.cpp')
STRACE_MAIN = [ 'strace/ioctlsort.cpp' ]
STRACE_SRC = [f for f in STRACE_ALL if not f in STRACE_MAIN] # all except main program

env = Environment(CC = CC,
                  CXX = CXX,
                  CPPDEFINES = [ CONFIG_ENDIAN ],
                  CFLAGS = STD_CFLAGS + DEBUG_FLAGS + WARN_FLAGS + WARN_CFLAGS,
                  CXXFLAGS =  STD_CXXFLAGS + DEBUG_FLAGS + WARN_FLAGS + WARN_CXXFLAGS,
                  CPPPATH = ['boost-extras', '.'] + NETTLE_INCLUDE + ARMA_INCLUDE + BOOST_INCLUDE)

# Ada
env.Append(ENV = {'PATH' : os.environ['PATH']},
           tools = ['Ada'],
           toolpath = ['scons-builders']
)

# def adb_action( target, source, env ):
# #    os.system( "rpcgen -c -o " + target_file + " " + source_file )
#     print((target, source, env))
# adb_builder = Builder(name = "Ada",
#                       action = adb_action,
#                       suffix = ".o",
#                       src_suffix = ".adb")
# env.Append(BUILDERS = [adb_builder])

# env.Object('small.adb')

env.Clone(LIBPATH = D_LIBPATH).Program('t_geom', ['t_geom.d'])

# UnTar
# env.Append(BUILDERS = {'UnTar' : unTarBuilder,
#                        'Ada' : AdaBuilder})
# external_package = 'packages/boost.tar.bz2'
# archive = env.UnTar(source=external_package)

# Caching/Memoization
CACHE_DIR = os.path.join(HOME, '.cache/scons')
try:
    os.mkdir(CACHE_DIR)
except os.error:
    pass
env.CacheDir(CACHE_DIR)
env.Decider('MD5-timestamp') # Choose either 'MD5', 'timestamp-newer', 'timestamp-match', 'MD5-timestamp'

def lowerMaybe(x):
    if x != None:
        return x.lower()
    else:
        return x

if (ARGUMENTS.get('release', 0) or
    'release' in [lowerMaybe(ARGUMENTS.get('BUILD_TYPE')),
                  lowerMaybe(ARGUMENTS.get('build')) ]):
    env.Append(CFLAGS = RELEASE_OPT_FLAGS)
    env.Append(CXXFLAGS = RELEASE_OPT_FLAGS)
    env.Append(CPPDEFINES = ['-DNDEBUG', '-DARMA_NO_DEBUG', '-DBOOST_DISABLE_ASSERTS'])
#    LibraryBuilder = env.SharedLibrary
    LibraryBuilder = env.StaticLibrary
else:
    env.Append(CFLAGS = DEBUG_FLAGS)
    env.Append(CXXFLAGS = DEBUG_FLAGS)
    env.Append(CPPDEFINES = ['-DDEBUG'])
    LibraryBuilder = env.StaticLibrary

# Configuration
conf = Configure(env)

def have_CHeader(conf, hdr):
    """Check C Header(s) HDR."""
    if isinstance(hdr, list):
        for h in hdr:
            have_CHeader(conf, h)
    elif conf.CheckCHeader(hdr):
        conf.env.Append(CPPDEFINES = '-DHAVE_' + re.sub(r'[^a-zA-Z_0-9]', '_', hdr).upper())

def have_CXXHeader(conf, hdr):
    """Check C++ Header(s) HDR."""
    if isinstance(hdr, list):
        for h in hdr:
            have_CXXHeader(conf, h)
    elif conf.CheckCXXHeader(hdr):
        conf.env.Append(CPPDEFINES = '-DHAVE_' + re.sub(r'[^a-zA-Z_0-9]', '_', hdr).upper())

# System Headers
have_CHeader(conf, 'sys/xattr.h')
have_CHeader(conf, 'sys/inotify.h')
# POSIX Asynchronous I/O
have_CHeader(conf, 'aio.h')
# OpenSSL
have_CHeader(conf, ['openssl/md5.h',
                    'openssl/sha.h',
                    'openssl/evp.h'])
# Nettle
have_CHeader(conf, ['nettle/md5.h',
                    'nettle/sha.h',
                    'nettle/sha2.h',
                    'nettle/sha3.h'])
# FFMPEG
have_CHeader(conf, ['libavformat/avformat.h',
               'libavformat/avcodec.h'])
# Boost
have_CXXHeader(conf, ['boost/lexical_cast.hpp'])
have_CXXHeader(conf, ['boost/serialization/access.hpp'])
# OpenGL
have_CHeader(conf, ['GL/gl.h', 'GL/glu.h', 'GL/glext.h', 'GL/glx.h', 'GL/glxext.h',
                    'X11/Xlib.h',
                    'GL/glew.h', 'GL/glxew.h'])
# Various
have_CHeader(conf, 'magic.h')
have_CHeader(conf, 'freeimage.h')
have_CHeader(conf, 'opencv/cv.h')
have_CHeader(conf, 'bfd.h')

# Functions
if conf.CheckFunc('mmap'):
    conf.env.Append(CPPDEFINES = '-DHAVE_MMAP')
if conf.CheckFunc('pread'):
    conf.env.Append(CPPDEFINES = '-DHAVE_PREAD')
if conf.CheckFunc('pwrite'):
    conf.env.Append(CPPDEFINES = '-DHAVE_PWRITE')

# cutils
cutils_src = [ 'msort.c', 'qsort.c', 'qsort_mt.c', 'reorg.c', 'utils.c', 'timing.c', 'aesc.c', 'binlog.c', 'perm.c',
               'inotify_utils.c', 'readline_utils.c', 'statutils.c', 'string_x.c', 'stdio_x.c', 'pathops.c', 'fkind.c', 'magic_x.c' ]
# Library
libcutils = LibraryBuilder('cutils', cutils_src)

# SemNet Library
libsemnet = LibraryBuilder('semnet', SEMNET_SRC)
libstrace = env.Clone(CPPDEFINES = '-DHAVE_CONFIG_H',
                      CPPPATH = ['strace',  '.', 'strace/linux', 'strace/linux/x86_64' ]).StaticLibrary('strace', STRACE_SRC)

# Object
iostream_x = env.Object('iostream_x.cpp')
ioredirect = env.Object('ioredirect.cpp')
layout = env.Object('geometry/layout.cpp')
rand_cxx = env.Object('rand.cpp')

env.Program('t_stl.out', ['t_stl.cpp'])
env.Program('t_strings.out', ['t_strings.cpp'])
env.Program('t_ranged_and_saturated.out', ['t_ranged_and_saturated.cpp'])
env.Program('t_maybe.out', ['t_maybe.cpp'])

KERNEL_VERSION = subprocess.check_output(['uname', '-r']).strip()
env.Clone(CPPPATH = [ '/usr/src/linux-headers-'+KERNEL_VERSION+'/include',
                      '/usr/src/linux-headers-'+KERNEL_VERSION+'/arch/x86/include'],
          LIBS = ['bfd']).Program('t_ptrace.out', ['t_ptrace.cpp'])

env.Clone(CPPPATH = OGLPLUS_INCLUDE + GLFW3_INCLUDE,
          LIBPATH = OGLPLUS_LIBPATH + GLFW3_LIBPATH,
          LIBS = ['glfw', 'GLEW', 'GL']).Program('t_oglplus.out', ['t_oglplus.cpp'])

env.Clone(LIBS = ['boost_serialization']).Program('t_boost_serialization.out',
                                                  ['t_boost_serialization.cpp',
                                                   iostream_x])

env.Clone(LINKFLAGS = '-fopenmp').Program('t_sort.out',
                                          ['t_sort.cpp', libcutils ],
                                          LIBS = [ 'rt', 'tbb', 'pthread'])

env.Program('t_poly.out',
            ['t_poly.cpp', 'poly.cpp', 'sinefit.cpp', libcutils ],
            LIBS = [ 'armadillo', 'pthread'],
            CPPPATH = NETTLE_INCLUDE + ARMA_INCLUDE,
            LIBPATH = NETTLE_LIBPATH + ARMA_LIBPATH + BOOST_LIBPATH)

env.Program('t_linterpol.out',
            ['t_linterpol.cpp' ],
            LIBS = [ 'armadillo', 'pthread'],
            CPPPATH = ARMA_INCLUDE,
            LIBPATH = ARMA_LIBPATH)

env.Program('t_armadillo.out',
            ['t_armadillo.cpp' ],
            LIBS = [ 'armadillo', 'pthread'],
            CPPPATH = NETTLE_INCLUDE + ARMA_INCLUDE,
            LIBPATH = NETTLE_LIBPATH + ARMA_LIBPATH + BOOST_LIBPATH)

env.Program('t_outer_sphere.out',
            ['t_outer_sphere.cpp' ])

env.Clone(CPPPATH = BOOST_ENDIAN_INCLUDE).Program('t_boost_endian.out',
                                                  ['t_boost_endian.cpp' ])

env.Clone(LIBS = ['boost_filesystem', 'boost_system']).Program('t_boost_fs.out',
                                                               ['t_boost_fs.cpp' ])

env.Program('t_type_traits.out',
            ['t_type_traits.cpp' ])

env.Program('t_boost_accumulators.out',
            ['t_boost_accumulators.cpp' ])

env.Program('t_boost_bimap.out',
            ['t_boost_bimap.cpp' ])

env.Program('t_boost_random.out',
            ['t_boost_random.cpp' ])

env.Program('t_show.out',
            ['t_show.cpp' ])

env.Program('t_median.out',
            ['t_median.cpp' ])

env.Clone(LIBS = [libcutils]).Program('t_histogram.out',
                                      ['t_histogram.cpp', ioredirect])

env.Program('t_rand.out', ['t_rand.cpp', rand_cxx])

env.Clone(LIBS = [ 'nettle', 'rt', 'crypto' ],
          CPPPATH = NETTLE_INCLUDE,
          LIBPATH = NETTLE_LIBPATH).Program('t_chash.out', ['t_chash.cpp'])

env.Clone(CPPPATH = BOOST_PROCESS_INCLUDE,
          LIBS = [libcutils, 'boost_filesystem', 'boost_system']).Program('t_timed.out',
                                                                          ['t_timed.cpp', ioredirect, iostream_x])

env.Clone(CPPPATH = BOOST_PROCESS_INCLUDE,
          LIBS = ['boost_filesystem', 'boost_system']).Program('t_boost_process.out',
                                                               ['t_boost_process.cpp'])

env.Program('t_geometry.out',
            ['t_geometry.cpp', 'geometry/box.cpp', layout])

env.Clone(LIBS = ['elf']).Program('t_libelf.out',['t_libelf.cpp'])

env.Program('t_semnet.out',
            [SEMNET_MAIN, 'chash.cpp', 'ffmpeg_x.cpp', 'udunits.cpp', 'vcs.cpp', ioredirect, 'libghthash/src/hash_functions.c', 'libghthash/src/hash_table.c' ],
            LIBS = [ libsemnet, libcutils, libstrace,
                     'rt', 'readline', 'magic', 'avformat', 'avcodec', 'freeimage', 'udunits2', 'crypto',
                     'boost_filesystem', 'boost_system',
                     'pthread', 'nettle', 'gmp'], # , 'boost_iostreams'
            LIBPATH = NETTLE_LIBPATH + ARMA_LIBPATH + BOOST_LIBPATH)

# env.Program('t_boost_concept_requires.out',
#                     ['t_boost_concept_requires.cpp' ])
