import drake

class GNUBuilder(drake.Builder):

  def __init__(self,
               cxx_toolkit,
               configure,
               targets,
               args = [],
               sources = [],
               configure_interpreter = None):
    self.__toolkit = cxx_toolkit
    self.__configure = configure
    self.__configure_interpreter = configure_interpreter
    self.__args = args
    self.__targets = targets
    drake.Builder.__init__(self, [configure] + sources, targets)

  def execute(self):
    if not self.cmd('Configure %s' % self.work_directory,
                    self.command_configure,
                    cwd = self.work_directory,
                    leave_stdout = True):
      return False
    if not self.cmd('Build %s' % self.work_directory,
                    self.command_build,
                    cwd = self.work_directory,
                    leave_stdout = True):
      return False
    for target in self.__targets:
      path = drake.Path(target.path())
      path.strip_prefix(self.work_directory)
      with drake.WritePermissions(target):
        cmd = self.__toolkit.rpath_set_command(target.path(), '.')
        if not self.cmd('Fix rpath for %s' % target.path(),
                        cmd):
          return False
    return True

  @property
  def command_configure(self):
    basename = str(self.__configure.path().basename())
    if self.__configure_interpreter is None:
      config = ['./%s' % basename]
    else:
      config = [self.__configure_interpreter, basename]
    return config + self.__args

  @property
  def command_build(self):
    return ['make', 'install']

  @property
  def work_directory(self):
    return str(self.__configure.path().dirname())


  def hash(self):
    return str(self.command_configure) + str(self.command_build)
