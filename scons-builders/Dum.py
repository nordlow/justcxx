import os.path
import SCons.Builder
import SCons.Node.FS
import SCons.Util

# binary tools to use
AdaCompilers = ['gcc']
AdaBinders   = ['gnatbind']
AdaLinkers   = ['gnatlink']

# file patterns
AdaBodySuffix = '.adb';
AdaHdrSuffix = '.ads';
AdaSuffixes = [AdaBodySuffix, AdaHdrSuffix]
AliSuffixes = ['.ali']
AdaScanExt = AdaSuffixes + AliSuffixes;

# source code patterns
pkgBodyRegex = re.compile("(?i)^\s*package\s+body\s+\w+\s+is\s*$");
procRegex = re.compile("(?i)^\s*procedure\s+\w+\s+is\s*$");
withRegex = re.compile("(?i)^\s*with\s+([\.\w]+[\s*,\s*[\.\w]+]*)\s*;");

# construct an uppercase list of the Ada suffixes
AdaUpperSuffixes = [];
for suffix in AdaSuffixes:
    AdaUpperSuffixes.append(string.upper(suffix));

# csccom = "$CSC $CSCFLAGS -out:${TARGET.abspath} $SOURCES"
# csclibcom = "$CSC -t:library $CSCLIBFLAGS $_CSCLIBPATH $_CSCLIBS -out:${TARGET.abspath} $SOURCES"


# McsBuilder = SCons.Builder.Builder(action = '$CSCCOM',
#                                    source_factory = SCons.Node.FS.default_fs.Entry,
#                                    suffix = '.exe')

# McsLibBuilder = SCons.Builder.Builder(action = '$CSCLIBCOM',
#                                    source_factory = SCons.Node.FS.default_fs.Entry,
#                                    suffix = '.dll')

def generate(env):
    """Register Builders and Construction Variables for the GNAT Compiler in Environment ENV."""

    AdaBuilder = SCons.Builder.Builder(name = "GNATCompile",
                                       action = '$CSCCOM',
                                       source_factory = SCons.Node.FS.default_fs.Entry,
                                       suffix = '.exe')

    env['BUILDERS']['Ada'] = AdaBuilder

    # env['BUILDERS']['CLILibrary'] = McsLibBuilder

    # env['CSC']        = 'mcs'
    # env['_CSCLIBS']    = "${_stripixes('-r:', CILLIBS, '', '-r', '', __env__)}"
    # env['_CSCLIBPATH'] = "${_stripixes('-lib:', CILLIBPATH, '', '-r', '', __env__)}"
    # env['CSCFLAGS']   = SCons.Util.CLVar('')
    # env['CSCCOM']     = SCons.Action.Action(csccom)
    # env['CSCLIBCOM']  = SCons.Action.Action(csclibcom)

def exists(env):
    return env.Detect(gnatmake)
