import drake

class GNUBuilder(drake.Builder):

  def __init__(self,
               cxx_toolkit,
               configure,
               targets,
               args = [],
               sources = []):
    self.__toolkit = cxx_toolkit
    self.__configure = configure
    self.__args = args
    self.__targets = targets
    drake.Builder.__init__(self, [configure] + sources, targets)

  def execute(self):
    return self.cmd('Build %s' % self.work_directory,
                    self.command,
                    cwd = self.work_directory,
                    leave_stdout = True)

  @property
  def command(self):
    basename = str(self.__configure.path().basename())
    config = ['./%s' % basename] + self.__args
    install = ['make', 'install']
    rpath = []
    for target in self.__targets:
      path = drake.Path(target.path())
      path.strip_prefix(self.work_directory)
      rpath.append(self.__toolkit.rpath_set_command(path, '.'))
    return tuple([config, install] + rpath)

  @property
  def work_directory(self):
    return str(self.__configure.path().dirname())

  def hash(self):
    return str(self.command)
