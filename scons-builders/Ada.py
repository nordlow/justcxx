__revision__ = ""

import re, string;

import os.path
from SCons.Script import *;
import SCons.Tool
import SCons.Defaults
import SCons.Util

False = 0;
True = 1;

# the binary tools to use
AdaCompilers = ['gcc']
AdaBinders   = ['gnatbind']
AdaLinkers   = ['gnatlink']

# the suffixes we are interested in
AdaBodySuffix = '.adb';
AdaHdrSuffix = '.ads';
AdaSuffixes = [AdaBodySuffix, AdaHdrSuffix]
AliSuffixes = ['.ali']

AdaScanExt = AdaSuffixes + AliSuffixes;

pkgBodyRegex = re.compile("(?i)^\s*package\s+body\s+\w+\s+is\s*$");
procRegex = re.compile("(?i)^\s*procedure\s+\w+\s+is\s*$");
withRegex = re.compile("(?i)^\s*with\s+([\.\w]+[\s*,\s*[\.\w]+]*)\s*;");

# Construct an uppercase list of the ada suffixes
AdaUpperSuffixes = [];
for suffix in AdaSuffixes:
    AdaUpperSuffixes.append(string.upper(suffix));


# File handle to use instead of creating one within printDebug
of = None;

import inspect;
def lineno():
    """Returns the current line number in our program."""
    return inspect.currentframe().f_back.f_lineno



def printDebug(debug, function, ln, text):
    global of;
    if debug:
        if of == None:
            of = open(File("#/out.txt").abspath, "w");
        of.write("%s(%s): %s\n" % (function, ln, text));



def getWithList(file):
    """
    Description:
        This will scan file, and return a list of all the items
        that are included with a "with" statement.

    Parameters:
        file - File to be scanned. Typically either an ".adb"
        or ".ads" file

    Returns:
        List of items included using a "with" statement
    """
    debug = True;
    printDebug(debug,
               inspect.currentframe().f_code.co_name,
               lineno(),
               "Entering(%s)" % file);

    retValue = [];

    # We cannot scan a file that doesn't exist :(
    if not os.path.exists(file):
        return retValue;

    # Loop through the file, and match using the "with"
    # regular expression
    for line in open(file):
        match = withRegex.match(line);
        if match != None:

            # You can "with" a comman separated list, so
            # split based on the comma, to get the list, and
            # strip off any whitespace
            for item in match.group(1).split(","):
                retValue.append(item.strip());

    printDebug(debug,
               inspect.currentframe().f_code.co_name,
               lineno(),
               "Returning %s" % retValue);

    return retValue;


def isBindTarget(file, env):
    """
    Description:
        This function will check file, and determine whether it is a
        target of the Bind() functionality or not.  We use the
        ADABINDPREFIX and ADABINDSUFFIX to determine if the file name
        matches the format.

    Parameters:
        file - String file name to check to see if it is a bind target
        env - build environment

    Returns:
        1 (True) if file is a bind target
        0 (False) Otherwise

    """

    hasPrefix = os.path.basename(file).startswith(env["ADABINDPREFIX"]);
    hasSuffix = os.path.basename(file).endswith(env["ADABINDSUFFIX"]);
    return hasPrefix and hasSuffix;


def isMainFile(file):
    """
    Description:
        Used to determine if file is a source file that
        contains the "main" function

    Parameters:
        file - File to be checked to determine if it is a "main" file

    Returns:
        1 (True) - If file is a "main" program
        0 (False) - If file is not a "main" program
    """
    file = str(file);

    # Not a main if the file doesn't exist yet :-P
    if not os.path.exists(file):
        return 0;

    # If the file does not have an extension, exit
    if AdaBodySuffix not in file:
        return 0;

    # If the extension is not an Ada body file
    if string.upper(os.path.splitext(file)[1]) != \
       string.upper(AdaBodySuffix):
        return 0;


    # Open the file.  If we find a procedure before a package body
    # declaration, then we are a main file, otherwise we are NOT a
    # main file
    for line in open(file):

        a = pkgBodyRegex.match(line);
        if a != None:
            return 0; # False

        a = procRegex.match(line);
        if a != None:
            return 1; # True

    return 0; # False


def gatherAliFiles(file, env, hintList=[]):
    """
    Description:
        NOTE: This really should be a target scanner that is
              ONLY called for the b~*.* files, and then it
              should figure out it's base file, etc..

        Called from the source scanner.  Given a main file, path
        list, env, and optional hintList, this recursivly called
        function will return a list of .ali files that file refers
        to.  This list of .ali files is determined by examining
        the "with"ed files that are referred to in file.

    Parameters:
        file - The file to be examined for ali file dependencies
        path - path parameter from the Scanner
        emv = The env parameter from the Scanner
        hintList - List used for recursive calls to limit the same
                   files from being processed multiple times.

    Returns:
        List of .ali files which file depends on.
    """
    debug = True;
    printDebug(debug,
               inspect.currentframe().f_code.co_name,
               lineno(),
               "Entering(%s, %s)" % (file, hintList));

    retValue = [];

    # Sanity check
    if not os.path.exists(file):
        printDebug(debug,
                   inspect.currentframe().f_code.co_name,
                   lineno(),
                   "***** %s DOES NOT EXIST *****" % file);
        return retValue;

    # Get a list of items that were "withed" in the file
    withList = getWithList(file);

    p = os.path.dirname(file);

    # Get a list of items in the directory
    fileDict = {};
    for d in os.listdir(str(p)):
        fileDict[string.upper(d)] = d;

    # Loop through the items that were "withed"
    for withedItem in withList:
        upperwith = string.upper(withedItem);

        # Search for UPPERWITH.ADB/ADS to see if they exist
        for ext in AdaUpperSuffixes:
            fn = upperwith + ext;

            printDebug(debug,
                       inspect.currentframe().f_code.co_name,
                       lineno(),
                       "fn = %s" % fn);

            # If the file is not in the directory, bail out
            if fn not in fileDict.keys():
                continue;

            # Construct the .ali file name, and add it as
            # a dependency to file
            aliFile = getAliName(fileDict[fn], env);
            upperAli = string.upper(aliFile.abspath);

            printDebug(debug,
                       inspect.currentframe().f_code.co_name,
                       lineno(),
                       "upperAli = %s" % upperAli);

            # If aliFile is empty, or it's in the hint
            # list, then keep going
            if aliFile == '' or upperAli in hintList:
                continue;

            hintList.append(upperAli);
            retValue = retValue + \
                       gatherAliFiles(os.path.join(str(p), fileDict[fn]),
                                      env,
                                      hintList);

            retValue.append(os.path.join(Dir('.').abspath, os.path.basename(aliFile.abspath)));




    for r in retValue:
        printDebug(debug,
                   inspect.currentframe().f_code.co_name,
                   lineno(),
                   "Returning %s" % (r));

    return retValue;





def getObjName(file, env, dirName=None):
    """
    Description:
        Convenience function used to retrieve a File object
        representing an Object which is a target of file

    Parameters:
        file - File that is the source for the Object
        env - build environment
        dirName - Optional directory name used to force
                  the destination of the Object. Defaults
                  to the same directory as file

    Returns:
        File object representing the Object file created
        from file or
        "" if file is not an Ada source file
    """
    debug = True;
    printDebug(debug,
               inspect.currentframe().f_code.co_name,
               lineno(),
               "Entering(%s)" % file);

    file = str(file);

    # Handle dirname
    if dirName == None:
        dirName = Dir(".").abspath;
    else:
        dirName = Dir(dirName).abspath;


    printDebug(debug,
               inspect.currentframe().f_code.co_name,
               lineno(),
               "dirName = %s" % dirName);

    # Sanity check on file extension
    if os.path.splitext(file)[1] not in AdaSuffixes:
        return "";

    fileBase = os.path.basename(os.path.splitext(file)[0]);

    obj = env['ADAOBJPREFIX'] + fileBase + env['ADAOBJSUFFIX'];
    obj = os.path.join(dirName, obj);
    printDebug(debug,
               inspect.currentframe().f_code.co_name,
               lineno(),
               "obj = %s" % obj);

    retValue = File(obj);

    printDebug(debug,
               inspect.currentframe().f_code.co_name,
               lineno(),
               "Returning(%s)" % (str(retValue.abspath)));
    return retValue;



def getAliName(file, env, dirName=None):
    """
    Description:
        Convenience function used to retrieve a File object
        representing a .ali file which is a target of file

    Parameters:
        file - File that is the source for the Object
        env - build environment
        dirName - Optional directory name used to force
                  the destination of the Object. Defaults
                  to the same directory as file

    Returns:
        File object representing the ali file created
        from file or
        "" if file is not an Ada source file
    """
    debug = True;
    printDebug(debug,
               inspect.currentframe().f_code.co_name,
               lineno(),
               "Entering(%s)" % file);

    file = str(file);

    # Handle dirname
    if dirName == None:
        dirName = Dir(".").abspath;
    else:
        dirName = Dir(dirName).abspath;

    # Sanity check on file extension
    if os.path.splitext(file)[1] not in AdaSuffixes:
        return "";

    fileBase = os.path.basename(os.path.splitext(file)[0]);

    ali = env['ALIPREFIX'] + fileBase + env['ALISUFFIX'];


    printDebug(debug,
               inspect.currentframe().f_code.co_name,
               lineno(),
               "./: %s    -      Ali: %s" % (Dir(".").abspath, ali));
    ali = os.path.join(dirName, ali);

    retValue = File(ali);

    printDebug(debug,
               inspect.currentframe().f_code.co_name,
               lineno(),
               "Returning(%s)" % (str(retValue.abspath)));

    return retValue;




def getBindFiles(file, env):

    retValue = [];
    baseFile = os.path.basename(os.path.splitext(file)[0]);
    ext = os.path.splitext(file)[1];

    if isBindTarget(file, env) or \
       ext != ".adb":
        return retValue;

    bodyFile = env['ADABINDPREFIX'] + baseFile + env['ADABINDSUFFIX'] + ".adb";
    specFile = env['ADABINDPREFIX'] + baseFile + env['ADABINDSUFFIX'] + ".ads";

    bodyFile = File("#/" + bodyFile);
    specFile = File("#/" + specFile);

    retValue.append(bodyFile);
    retValue.append(specFile);

    return retValue;


def filterAdaSources(source):
    retValue = [];
    hintList = [];

    for src in source:
        hint = string.upper(os.path.basename(src.path));
        hint = os.path.splitext(hint)[0];

        if "." not in src.abspath:
            continue;

        if os.path.splitext(src.abspath)[1] in AdaSuffixes and \
           hint not in hintList:
            hintList.append(hint);
            retValue.append(File(src));

    return retValue;






def adaEmitter(target, source, env):
    debug = True;

    printDebug(debug,
               inspect.currentframe().f_code.co_name,
               lineno(),
               "Entering adaEmitter()");

    printDebug(debug,
               inspect.currentframe().f_code.co_name,
               lineno(),
               "Dir(\".\") = %s" % Dir(".").abspath);

    for s in source:
        printDebug(debug,
                   inspect.currentframe().f_code.co_name,
                   lineno(),
                   "Begin Source: %s" % s.abspath);
    for t in target:
        printDebug(debug,
                   inspect.currentframe().f_code.co_name,
                   lineno(),
                   "Begin Target: %s" % t.abspath);

    source = filterAdaSources(source);

    newTarget = [];
    bindFilesList = [];
    for s in source:

        # Figure out the .ali and .o files
        obj = getObjName(s, env)
        newTarget.append(obj);

        ali = getAliName(s, env)
        newTarget.append(ali);

        if isMainFile(s.srcnode().abspath):

            # Figure out the bind files (if any)
            bindFiles = getBindFiles(s.path, env);
            bindFilesList = bindFilesList + bindFiles;

            aliFiles = gatherAliFiles(s.srcnode().abspath, env, []);

            depList = [obj, ali] + bindFiles;
            for dep in depList:
                printDebug(debug,
                           inspect.currentframe().f_code.co_name,
                           lineno(),
                           dep);

            printDebug(debug,
                       inspect.currentframe().f_code.co_name,
                       lineno(),
                       "Depends On");

            for af in aliFiles:
                printDebug(debug,
                           inspect.currentframe().f_code.co_name,
                           lineno(),
                           af);

            depList = [obj, ali] + bindFiles;
            Depends(depList, aliFiles);




    # Handle the emission of the bind targets by calling
    # ourselves recursively
    if bindFilesList != []:
        bindSource = [];
        bindTarget = [];

        # Turn all the bind files into File() objects
        for bindFile in bindFilesList:
            bindSource.append(bindFile);
            bindTarget.append(getObjName(bindFile, env));

        t, s = adaEmitter(bindTarget, bindSource, env);
        newTarget = newTarget + t;


    for s in source:
        printDebug(debug,
                   inspect.currentframe().f_code.co_name,
                   lineno(),
                   "Emitting Source: %s" % s.abspath);
    for t in newTarget:
        printDebug(debug,
                   inspect.currentframe().f_code.co_name,
                   lineno(),
                   "Emitting newTarget: %s" % t.abspath);


    printDebug(debug,
               inspect.currentframe().f_code.co_name,
               lineno(),
               "Exiting ");
    return newTarget, source;




def adaScanner (node, env, path):

    debug = True;
    printDebug(debug,
               inspect.currentframe().f_code.co_name,
               lineno(),
               "Scanning %s" % str(node));

    retValue = [];
    withList = [];

    withList = getWithList(str(node))

    # Go through the paths
    for p in path:

        # Make sure it exists
        if not os.path.exists(p.abspath):
            continue;

        # Get a file listing
        fileDict = {};
        for d in os.listdir(p.abspath):
            fileDict[string.upper(d)] = d;

        # Add dependency to corresponding body or header
        # (if they exist)
        for ext in AdaSuffixes:

            # Don't build ourselves
            if os.path.splitext(str(node))[1] == ext:
                continue;

            nodeUpper = os.path.basename(str(node));
            nodeUpper = string.upper(os.path.splitext(nodeUpper)[0]);
            nodeUpper = nodeUpper + string.upper(ext);

            # Add dependency to our sister file
            if nodeUpper in fileDict.keys():
                retValue.append(os.path.join(p.abspath, fileDict[nodeUpper]));


        # Add dependencies to everything "with"ed
        for withedItem in withList:
            upperwith = string.upper(withedItem);

            for ext in AdaUpperSuffixes:
                fn = upperwith + ext;
                if fn in fileDict.keys():
                    retValue.append(os.path.join(p.abspath, fileDict[fn]));

    for r in retValue:
        printDebug(debug,
                   inspect.currentframe().f_code.co_name,
                   lineno(),
                   "Returning: %s" % (r));
    return retValue;


def adaAction(target, source, env):
    debug = True;
    printDebug(debug,
               inspect.currentframe().f_code.co_name,
               lineno(),
               "Building %s from %s " % (target[0].path, source[0].path));

    source = filterAdaSources(source);

    # Compile the Ada file
    cmd = env.subst(env['ADACOM'], target=target, source=source);
    retValue = env.Execute(cmd);

    if retValue != 0:
        return retValue;

    bindFiles = [];
    # Figure out the bind files
    if isMainFile(source[0].srcnode().abspath):
        bindFiles = getBindFiles(source[0].abspath, env);

    if bindFiles != []:

        # Determine the target and source for binding
        bindTgt = File(bindFiles[0]);
        bindSrc = File(target[1]);

        # Construct the command and execute it
        cmd = env.subst(env['ADABINDCOM'], target=bindTgt, source=bindSrc);
        retValue = env.Execute(cmd);

        if retValue != 0:
            return retValue;  adaScanner

        # Recursively call the builder to build the source file
        compTgt = getObjName(bindFiles[0], env, os.path.dirname(target[0].abspath));
        retValue = adaAction([compTgt], [bindFiles[0]], env);

        # Remove the bind source files to emulate what GNATLINK does
        for bf in bindFiles:
            os.remove(bf.abspath);

        if retValue != 0:
            return retValue;


    printDebug(debug,
               inspect.currentframe().f_code.co_name,
               lineno(),
               "Returning: %s" % (retValue));

    return retValue;



def adaActionString(target, source, env):
    print "*** Compiling %s ***" % source[0];


def generate(env):
    """
    Add Builders and construction variables for the gnat compiler to
    an Environment.
    """

    env['LINK'] = 'gcc';

    # Figure out some fancy way to set a sane adalib path here
    env['ADALIBPATH'] = "/usr/bin";

    # Hardcoded the libpath here for cygwin on windows. Not ideal, and
    # I really ought to do this a different way.
    libPaths = \
        Split("""
            C:\\cygwin\\lib\\gcc\\i686-pc-cygwin\\3.4.4\\adalib
        """);

    # Parse the ADA_OBJECTS_PATH environment variable for additional
    # locations for Ada libraries
    for lib in os.environ.get('ADA_OBJECTS_PATH',"").split(os.path.pathsep):
        libPaths += lib;

    env.Append(LIBPATH = libPaths);
    env.Append(LIBS = ['gnat']); # Parse the ADA_INCLUDE_PATH environment variable for additional
    # locations for Ada include files
    env['ADAPATH'] = os.environ.get('ADA_INCLUDE_PATH', "").split(os.path.pathsep);

    # Compile, Bind, and Link commands
    env['ADA']        = AdaCompilers[0];
    env['ADABIND']    = AdaBinders[0];

    # Flags passed to the compiler
    env['ADAFLAGS']   = ''
    env['ADABINDFLAGS']   = ''
    env['ADALINKFLAGS']   = ''

    # Our build commands
    env['ADACOM']     = '$ADA $ADAFLAGS -c -o $TARGET.posix  $SOURCES.posix'
    env['ADASHCOM']     = '$ADA $ADAFLAGS -c -o $TARGET.posix  $SOURCES.posix'
    env['ADABINDCOM'] = '$ADABIND $ADABINDFLAGS $SOURCES.posix'
    env['ADALINKCOM'] = '$ADALINK $ADALINKFLAGS -o $TARGET.posix $SOURCES.posix'

    # Obj information
    env['ADAOBJPREFIX'] = '';
    env['ADAOBJSUFFIX'] = '.o';

#    # Include prefix/suffix
#    env['ADAINCPREFIX']  = '-I'
#    env['ADAINCSUFFIX']  = ''

    # Generated .ali file prefix/suffix
    env['ALIPREFIX'] = ''
    env['ALISUFFIX'] = AliSuffixes[0]

    # Generated bind file prefix/suffix
    env['ADABINDPREFIX'] = 'b~'
    env['ADABINDSUFFIX'] = ''


    # Add the scanner
    adaScan = Scanner(name = 'adaScanner',
                      function = adaScanner,
                      path_function = SCons.Scanner.FindPathDirs('ADAPATH'),
                      skeys = AdaScanExt)

    # Associate this scanner with the extensions in AdaScanExt
    for ext in AdaScanExt:
        SourceFileScanner.add_scanner(ext, adaScan)

    static_obj, shared_obj = SCons.Tool.createObjBuilders(env)

    # Add handling for the source suffixes
    for suffix in AdaSuffixes:
        static_obj.add_action(suffix, Action(adaAction, adaActionString))
        shared_obj.add_action(suffix, Action(adaAction, adaActionString))
        static_obj.add_emitter(suffix, adaEmitter)
        shared_obj.add_emitter(suffix, adaEmitter)


def exists(env):
        printDebug(True, "", "", env.Detect(AdaCompilers))
        return env.Detect(AdaCompilers)
