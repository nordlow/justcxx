import os
#import SCons.Action
import SCons.Node

def UnTarEmitter(target, source, env):
    assert len(target) <= 1 # Until multiple target is managed
    for node in target:
        env.Clean(node, node)
    return target, source

def UnTar(target, source, env):
    # Code to build "target" from "source" here
    import tarfile
    sourceTar = tarfile.open(source[0].get_abspath(),'r')
    sourceTar.extractall(path=target[0].get_abspath())
    sourceTar.close()
    return None

def UnTarString(target, source, env):
    """ Information string for UnTar """
    return 'Extracting %s' % os.path.basename (str (source[0]))

def generate(env):
    action  = SCons.Action.Action(UnTar, UnTarString)
    builder = env.Builder(
            action         = action,
            emitter        = UnTarEmitter,
            target_factory = env.Dir,
            )
    env.Append(BUILDERS = {'UnTar' : builder})
