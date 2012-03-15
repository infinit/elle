# -*- encoding: utf-8 -*-

import os

def which(name):
    """Check for existence of an executable in the path and returns
    its absolute path when found, or None otherwise.

    >>> which('sh')
    "/bin/ls"
    >>> which('haters_gonna_hate')
    >>>

    """
    dirs = os.environ.get('PATH', '').split(':')
    for dir_ in dirs:
        path = os.path.join(dir_, name)
        if os.access(path, os.X_OK):
            return path
    return None
