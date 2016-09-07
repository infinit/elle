# Copyright (C) 2014, Quentin "mefyl" Hocquet
#
# This software is provided "as is" without warranty of any kind,
# either expressed or implied, including but not limited to the
# implied warranties of fitness for a particular purpose.
#
# See the LICENSE file for more information.

import drake
import math
import os
import shutil

from itertools import chain

class Packager(drake.Builder):

  def __init__(self,
               basename,
               attributes,
               sources,
               path,
               destination = '.',
               preload = None,
               cleanup_source_directory = True):
    path = drake.Path(path)
    self.__destination = drake.Path(destination)
    basename = drake.Path(basename)
    self.__target = drake.node(self.__destination / basename)
    self.__destination = drake.path_build(self.__destination)
    self.__path = drake.path_build(path)
    self.__preload = preload
    self.__attrs = attributes
    self.__control = drake.node(path / 'DEBIAN/control')
    if preload is not None:
      sources += [preload]
    self.__cleanup_source_directory = cleanup_source_directory
    super().__init__(
      sources, [self.__control, self.__target])

  @property
  def path(self):
    return self.__path

  def execute(self):
    if 'Installed-Size' not in self.__attrs:
      self.__attrs['Installed-Size'] = math.ceil(sum(
        os.path.getsize(os.path.join(p, f))
        for p, _, files in os.walk(str(self.__path))
        for f in files) / 1024)
    with open(str(self.__control.path()), 'w') as f:
      for k, v in self.__attrs.items():
        print('%s: %s' % (k, v), file = f)
    if self.__cleanup_source_directory:
      self.cleanup_source_directory(self.__path)
    os.chmod(str(self.__path / 'DEBIAN'), 0o755)
    os.chmod(str(self.__path / 'DEBIAN/control'), 0o644)
    try:
      os.chmod(str(self.__path / 'DEBIAN/postinst'), 0o755)
    except:
      pass
    env = {
      'PATH': os.environ['PATH'],
    }
    if self.__preload is not None:
      path = drake.path_root() / self.__preload.path()
      env['LD_PRELOAD'] = str(path)
    return self.cmd('Package %s' % self.__target,
                    self.command, env = env)

  @property
  def command(self):
    chown = ['chown', '-R', 'root:0', self.__path]
    chmod = ['chmod', '-R', 'a+rX', self.__path]
    dpkg = ['dpkg-deb', '-b', self.__path, self.__destination]
    import pipes
    def escape(cmd):
      return (pipes.quote(str(a)) for a in cmd)
    res = ['fakeroot', 'sh', '-e', '-c']
    import sys
    # OS X requires that the commands passed to fakeroot are properly quoted.
    if sys.platform == 'darwin':
      res.append(pipes.quote(' '.join(chain(escape(chown),
                                            [';'],
                                            escape(chmod),
                                            [';'],
                                            escape(dpkg)))))
    else:
      res.append(' '.join(chain(escape(chown),
                                [';'],
                                escape(chmod),
                                [';'],
                                escape(dpkg))))
    return res

  @property
  def package(self):
    return self.__target
