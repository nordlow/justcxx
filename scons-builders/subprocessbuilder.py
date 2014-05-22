# -*- coding: UTF8 -*-

import os
import subprocess

from racy.rutils import is_iterable

def SubProcessBuilder(target, source, env, command, args, pwd, path = []):

    if is_iterable(path):
        path = os.pathsep.join(path)
    path = os.pathsep.join([path, os.environ['PATH']])

    cmd = [ env.WhereIs(command, path=path) ]

    cmd.extend(args)

    environment = {}
    for k,v in env['ENV'].items():
        environment[k] = str(v)

    cmd = filter(lambda x:x, cmd)

    stdout = subprocess.PIPE

    process = subprocess.Popen(
                cmd,
                cwd = pwd,
                #stdout = stdout,
                env = environment
                )

    process.communicate()

    return process.returncode


def SubProcessString(target, source, env):
    return env.subst('${SUBPROCESSPREFIXSTR}')
