import os
import stat
import sys

_PATH = os.environ.get('PATH', '').split(os.pathsep)
_IS_WINDOWS = sys.platform.lower().startswith('win')

def is_executable(p, *path):
  path = os.path.join(p, *path)
  return os.path.exists(path) and \
    not os.path.isdir(path) and \
    os.stat(path)[stat.ST_MODE] & stat.S_IXUSR

def which(binary):
  if os.path.isabs(binary) and is_executable(binary):
    return binary
  for dir_ in _PATH:
    exe = os.path.join(dir_, binary)
    if is_executable(exe):
      return exe
  if _IS_WINDOWS and not binary.lower().endswith('.exe'):
    return which(binary + '.exe')
  return None
