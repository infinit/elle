import drake
from functools import lru_cache
import os

class CMakeBuilder(drake.Builder):

  def __init__(self, toolkit, srcs, dsts, vars,
               targets = None, path_to_cmake_source = None):
    '''
    `srcs`: what we depend upon.
    `dsts`: what will be built.
    `vars`: dict variables passed to cmake via `-D`.
    `targets`: list of Makefile targets.
    `path_to_cmake_source`: path to the directory containing the CMakeFile.
    '''
    self.__toolkit = toolkit
    self.__vars = vars
    self.__prefix = drake.Drake.current.prefix
    self.__path_to_cmake_source = \
        drake.Path(path_to_cmake_source) if path_to_cmake_source \
        else drake.path_source() / self.__prefix
    self.__env = dict(os.environ)
    self.__env.update({
      'CC': self.toolkit.c,
      'CXX': self.toolkit.cxx,
    })
    self.__cmake_cache = drake.node('CMakeCache.txt')
    self.__targets = targets
    # cmake 3 compat
    self.__vars.update({'CMAKE_SYSTEM_PROCESSOR': 'x86_64'})
    if self.toolkit.os is drake.os.windows:
      self.__vars.update({
        'CMAKE_ASM_NASM_COMPILER': self.toolkit.cxx[0:-3] + 'as',
        'CMAKE_RC_COMPILER': self.toolkit.cxx[0:-3] + 'windres',
        'CMAKE_SYSTEM_NAME': 'Windows',
      })
    dsts.append(self.__cmake_cache)
    # Call __init__ last, make __cmake_cache is declared a dsts, so
    # that it has a build-tree path, not a source-tree one.
    super().__init__(srcs = srcs, dsts = dsts)

  @property
  def toolkit(self):
    return self.__toolkit

  def execute(self):
    # cmake fails if the cache was moved.
    cpath = str(self.__cmake_cache.path())
    if os.path.exists(cpath):
      os.unlink(cpath)
    if not self.cmd(' '.join(self.cmake_cmd),
                    self.cmake_cmd,
                    cwd = self.__prefix,
                    env = self.__env):
      return False
    if self.__targets is None:
      return self.cmd('make', self.make_cmd, cwd = self.__prefix)
    for t in self.__targets:
      if isinstance(t, str):
        tgt = t
        wd = ''
      else:
        tgt = t[1]
        wd = t[0]
      if not self.cmd('make %s' % tgt, ['make', tgt], cwd = self.__prefix / wd):
        return False
    return True

  @property
  @lru_cache(maxsize=1)
  def cmake_cmd(self):
    cmd = ['cmake']
    for k, v in sorted(self.__vars.items()):
      cmd.append('-D%s=%s' % (k, v))
    cmd.append(str(self.__path_to_cmake_source))
    return cmd

  @property
  def make_cmd(self):
    return ['make']
  def hash(self):
    return (self.cmake_cmd, self.make_cmd)
  def __str__(self):
    return '%s' % (self.__class__.__name__)
