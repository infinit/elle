import drake

class Packager(drake.Builder):

  def __init__(self,
               basename,
               version,
               dist,
               arch,
               rpm_build_path,
               sources,
               destination = '.'):
    path = drake.path_build(rpm_build_path)
    self.__spec = path / 'SPECS' / ('%s.spec' % basename)
    self.__build_root = path / 'BUILDROOT'
    rpm_name = '%(base)s-%(ver)s-1.%(dist)s.%(arch)s.rpm' % \
      {'base': basename, 'ver': version, 'dist': dist, 'arch': arch}
    self.__build_target = path / 'RPMS' / arch / rpm_name
    self.__destination = destination
    self.__target = drake.node(drake.path_build(destination) / rpm_name)
    super().__init__(sources, [self.__target])

  def execute(self):
    return self.cmd('Package %s' % self.__target, self.command)

  @property
  def command(self):
    rpm_build = [
      'rpmbuild', '-bb', str(self.__spec),
      '--buildroot', str(self.__build_root)]
    cp = ['cp', str(self.__build_target), self.__destination]
    return rpm_build + [';'] + cp

  @property
  def package(self):
    return self.__target
