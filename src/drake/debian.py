import drake
import itertools
import os

class Packager(drake.Builder):

  def __init__(self, basename, sources, path, destination = '.',
               preload = None):
    self.__destination = drake.Path(destination)
    basename = drake.Path(basename)
    self.__target = drake.node(self.__destination / basename)
    self.__destination = drake.path_build(self.__destination)
    self.__path = drake.path_build(path)
    self.__preload = preload
    if preload is not None:
      sources += [preload]
    super().__init__(sources, [self.__target])

  def execute(self):
    os.chmod(str(self.__path / 'DEBIAN'), 0o755)
    os.chmod(str(self.__path / 'DEBIAN/control'), 0o644)
    try:
      os.chmod(str(self.__path / 'DEBIAN/postinst'), 0o755)
    except:
      pass
    env = {}
    if self.__preload is not None:
      path = drake.path_root() / self.__preload.path()
      env['LD_PRELOAD'] = str(path)
    return self.cmd('Package %s' % self.__target,
                    self.command, env = env)

  @property
  def command(self):
    chown = ['chown', '-R', 'root:root', self.__path]
    chmod = ['chmod', '-R', 'a+rX', self.__path]
    dpkg = ['dpkg-deb', '-b', self.__path, self.__destination]
    import pipes
    def escape(cmd):
      return (pipes.quote(str(a)) for a in cmd)
    return ['fakeroot', 'sh', '-e', '-c',
            ' '.join(itertools.chain(escape(chown),
                                     [';'],
                                     escape(chmod),
                                     [';'],
                                     escape(dpkg)))]

  @property
  def package(self):
    return self.__target
