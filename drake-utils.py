import drake
import drake.cxx
import drake.git
import itertools
import subprocess

def _default_make_binary():
  from drake.which import which
  to_try = [
    'make',
    'gmake',
    'mingw32-make',
    'mingw64-make',
  ]
  for binary in to_try:
    path = which(binary)
    if path is not None:
      return path

_DEFAULT_MAKE_BINARY = _default_make_binary()

class GNUBuilder(drake.Builder):

  def __init__(self,
               cxx_toolkit,
               targets = [],
               configure: """Configure script path (or None if no configure
                             step is needed)""" = None,
               working_directory: "Deduced from configure" = None,
               configure_args: "Arguments of the configure script" = [],
               sources = [],
               make_binary: "Make binary" = _DEFAULT_MAKE_BINARY,
               makefile: "Makefile filename, used if not None" = None,
               build_args: "Additional arguments for the make command" = ['install'],
               additional_env: "Additional environment variables" = {},
               configure_interpreter = None,
               patch = None):
    self.__toolkit = cxx_toolkit
    self.__configure = configure
    self.__configure_interpreter = configure_interpreter
    self.__configure_args = configure_args
    self.__targets = list(targets)
    self.__make_binary = make_binary
    self.__makefile = makefile
    self.__build_args = build_args
    self.__env = {}
    self.__env.update(additional_env)
    self.__patch = patch
    if make_binary is not None:
        self.__env.setdefault('MAKE', make_binary.replace('\\', '/'))
    if working_directory is not None:
        self.__working_directory = working_directory
    else:
        if self.__configure is None:
            raise Exception(
                "Cannot deduce the working directory (no configure script)"
            )
        self.__working_directory = self.__configure.path().dirname()
    drake.Builder.__init__(
      self,
      (configure is not None and [configure] or []) + sources,
      self.__targets)

  def execute(self):
    env = dict(self.__env)
    env.update(os.environ)
    with drake.CWDPrinter(drake.path_root() / drake.path_build() / self.work_directory):
      # Patch
      if self.__patch is not None:
        patch_path = str(drake.path_root() / self.__patch.path())
        patch_cmd = ['patch', '-N', '-p', '1', '-i', patch_path],
        if not self.cmd('Patch %s' % self.work_directory,
                        patch_cmd,
                        cwd = self.work_directory):
          return False
      # Configure step
      if self.__configure is not None:
         if not self.cmd('Configure %s' % self.work_directory,
                         self.command_configure,
                         cwd = self.work_directory,
                         env = env,
                         leave_stdout = False):
             return False
      # Build step
      if not self.cmd('Build %s' % self.work_directory,
                      self.command_build,
                      cwd = self.work_directory,
                      env = env,
                      leave_stdout = False):
        return False
    for target in self.__targets:
      path = target.path().without_prefix(self.work_directory)
      if not isinstance(target, drake.cxx.DynLib):
        continue
      with drake.WritePermissions(target):
        cmd = self.__toolkit.rpath_set_command(target.path(), '.')
        if not self.cmd('Fix rpath for %s' % target.path(),
                        cmd):
          return False
        if self.__toolkit.os is drake.os.macos:
          cmd = ['install_name_tool',
                 '-id', '@rpath/%s' % target.name().basename(),
                  str(target.path())]
          if not self.cmd('Fix rpath for %s' % target.path(), cmd):
            return False
          lib_dependecies = self.parse_otool_libraries(target.path())
          for dep in lib_dependecies:
            if dep.basename() in (t.path().basename() for t in self.__targets):
              cmd = [
                'install_name_tool',
                '-change',
                str(dep),
                '@rpath/%s' % dep.basename(),
                str(target.path()),
              ]
              if not self.cmd('Fix dependency name for %s' % target.path(), cmd):
                return False
    return True

  def parse_otool_libraries(self, path):
    command = ['otool', '-L', str(path)]
    return [drake.Path(line[1:].split(' ')[0])
            for line
            in subprocess.check_output(command).decode().split('\n')
            if line.startswith('\t')]

  @property
  def command_configure(self):
    if self.__configure is None:
        return None
    basename = str(self.__configure.path().basename())
    if self.__configure_interpreter is None:
      config = ['./%s' % basename]
    else:
      config = [self.__configure_interpreter, basename]
    return config + self.__configure_args

  @property
  def command_build(self):
    if self.__makefile is not None:
      return [self.__make_binary, '-f', self.__makefile, 'install'] + self.__build_args
    return [self.__make_binary] + self.__build_args

  @property
  def work_directory(self):
    return str(self.__working_directory)


  def hash(self):
    env = {}
    env.update(self.__env)
    env.pop('DRAKE_RAW', '1')
    return ''.join([
      str(self.command_configure),
      str(self.command_build),
      str(tuple(sorted(env))),
    ])

  def __str__(self):
    return '%s(%s)' % (self.__class__.__name__, self.__working_directory)

class VersionGenerator(drake.Builder):

  def __init__(self, output, git = None):
    git = git or drake.git.Git()
    drake.Builder.__init__(self, [git], [output])
    self.__git = git
    self.__output = output

  def execute(self):
    self.output('Generate %s' % self.__output.path())
    chunks = {}
    version = self.__git.description()
    chunks['version'] = version
    chunks['major'], chunks['minor'], chunks['subminor'] = \
      map(int, version.split('-')[0].split('.'))
    with open(str(self.__output.path()), 'w') as f:
      variables = (self._variable(*item) for item in chunks.items())
      for line in itertools.chain(
          self._prologue(), variables, self._epilogue()):
        print(line, file = f)
    return True

  def _prologue(self):
    return iter(())

  def _epilogue(self):
    return iter(())

  def _variable(self, name, value):
    raise NotImplementedError()
