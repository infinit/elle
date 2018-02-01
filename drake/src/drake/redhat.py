import drake
import os
import shutil

class Packager(drake.Builder):

  def __init__(self,
               basename,
               version,
               dist,
               arch,
               top_dir,
               sources,
               destination = '.'):
    if '-' in version:
      raise Exception('RPM cannot have "-" in version')
    self.__top_dir = drake.Path(os.getcwd()) / top_dir
    self.__spec = top_dir / 'SPECS' / ('%s.spec' % basename)
    self.__arch = arch
    self.__dist = dist
    self.__rpm_name = '%(base)s-%(ver)s-1.%(dist)s.%(arch)s.rpm' % \
      {'base': basename, 'ver': version, 'dist': dist, 'arch': self.__arch}
    self.__destination = drake.Path(destination)
    self.__target = drake.node('%s/%s' % (destination, self.__rpm_name))
    all_sources = list()
    for s in sources:
      all_sources.append(s)
      if isinstance(s, drake.cxx.Executable):
        for d in s.dependencies_recursive:
          all_sources.append(d)
    super().__init__(all_sources, [self.__target])

  def execute(self):
    rpm_dir = '%s/RPMS' % self.__top_dir
    if os.path.exists(rpm_dir):
      shutil.rmtree(rpm_dir)
    install_dir = '%s/BUILDROOT' % self.__top_dir
    for root, _, files in os.walk(install_dir):
      for f in files:
        f_path = ('%s/%s' % (root, f)).replace('%s/' % os.getcwd(), '', 1)
        if f_path not in [str(s) for s in self.sources()]:
          os.remove(str(f_path))
    for node in self.sources():
      if isinstance(node, drake.cxx.Executable):
        if not self.cmd('Undo prelink %s' % node, self.undo_prelink_cmd(node)):
          return False
    os.makedirs(rpm_dir)
    if not self.cmd('Package %s' % self.__rpm_name, self.rpm_build_cmd(dir)):
      return False
    return self.cmd('Copy %s' % self.__target, self.cp_rpm_cmd(dir))

  def undo_prelink_cmd(self, path):
    return ['prelink', '-u', str(path)]

  def rpm_build_cmd(self, root_dir):
    return [
      'rpmbuild', '-bb', str(self.__spec),
      '--define', '_topdir %s' % self.__top_dir,
      '--define', 'dist .%s' % self.__dist,
      '--buildroot', '%s/BUILDROOT' % self.__top_dir,
    ]

  def cp_rpm_cmd(self, root_dir):
    source = '%(top_dir)s/RPMS/%(arch)s/%(name)s' % \
      {'top_dir': self.__top_dir, 'arch': self.__arch, 'name': self.__rpm_name}
    return ['cp', source, str(self.__destination)]

  @property
  def package(self):
    return self.__target
