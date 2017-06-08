# Base for a Go module.
#
# This module contains the bare bones for a Go Module. It lacks a lot of
# functionalities but defines the base classes the final version should
# have.
#
# TODO:
# - Add tests
#   - Basics
#   - Cross-compilation
# - Discover current architecture
# - Support all Go specific environment
# - Handle package testing

import collections
import copy
import drake
import drake.cxx
import os
import subprocess

from drake.which import which
from os import environ
from orderedset import OrderedSet

class Config:
  """
  A Config represents a list of options to build a Go executables.
  """
  def __init__(self, config = None):
    """
    Create a Config from scrtach or from an existing configuration.

    :param config: An optional configuration.
    :type config: Config
    """
    self.__include_paths = OrderedSet()
    self.__ldflags = OrderedSet()
    self.__tags = OrderedSet()
    if config:
      self += config

  @property
  def include_paths(self):
    """
    Return the include paths as a list.

    :return: The list of include paths
    :rtype: list of str
    """
    return list(self.__include_paths)

  def add_include_paths(self, paths):
    """
    Add include paths at the end of the current include paths.

    :param paths: A list of paths
    :type paths: list of str
    """
    collections.deque(map(self.__include_paths.add, paths))

  def add_include_path(self, path):
    """
    Add an include path at the end of the current include paths.

    :param path: A path
    :type path: str
    """
    self.add_include_paths([path])
    return self

  @property
  def ldflags(self):
    """
    Return the ldflags as a list.

    :return: The list of ldflags
    :rtype: list of str
    """
    return list(self.__ldflags)

  def add_ldflags(self, flags):
    """
    Add ldflags at the end of current ldflags.

    :param flags: A list of flags.
    :type flags: list of str

    :return: self.
    """
    collections.deque(map(self.__ldflags.add, flags))
    return self

  def add_ldflag(self, flag):
    """
    Add a single ldflag at the end of the current ldflag.

    :param flag: An ldflag.
    :type flag: str

    :return: self
    """
    return self.add_ldflags([flag])

  @property
  def tags(self):
    """
    Return the tags as a list.

    :return: The lisf of tags
    :rtype: list of str
    """
    return list(self.__tags)

  def add_tags(self, tags):
    """
    Add tags at the end of current tags.

    :param flags: A list of flags.
    :type flags: list of str

    :return: self.
    """
    collections.deque(map(self.__tags.add, tags))
    return self

  def add_tag(self, tag):
    """
    Add a single tag at the end of the current tag.

    :param flag: An tag.
    :type flag: str

    :return: self
    """
    return self.add_tags([tag])

  def __add__(self, configuration):
    self.add_include_paths(configuration.include_paths)
    self.add_tags(configuration.tags)
    self.add_ldflags(configuration.ldflags)
    return self

  def hash(self):
    """
    Return the hash of the Config used for dependency computation.

    :return: Hash of the Config.
    :rtype: dict
    """
    return {
      'include_paths': self.include_paths,
      'tags': self.tags,
      'ldflags': self.ldflags,
    }


class Toolkit:
  """
  A Toolkit represents an abstraction of a Go environment.

  It'is designed to bind the GO* environment variables and offers a ways to get
  the go command to run or directly run them.
  """

  properties = ["path", "root", "os", "arch"]

  def __init__(self,
               tk = None,
               path = None,
               root = None,
               os = None,
               arch = None,
               cxx_toolkit = None):
    """
    Create a toolkit or clone an existing one.

    :param tk: The toolkit to clone. If specified, other arguments must be None
    :type tk: Toolkit
    :param path: The home of your go environment (override GOPATH).
    :type path: str
    :param root: The root of your go installation (override GOROOT).
    :type root: str
    :param os: The target os for cross-compilation (override GOOS).
    :type os: str
    :param arch: The target arch for cross-compilation (override GOARCH).
    :type arch: str

    Example:

    t = Toolkit(os = "windows", arch = "amd64")
    print(t.os)
    > "windows"
    print(t.env)
    {"GOOS": "windows", "GOARCH": "amd64"}

    t2 = Toolkit(t)
    print(t2.arch)
    > "amd64"
    """
    if isinstance(tk, Toolkit):
      assert all(a is None for a in [path, root, os, arch])
      return super().__init__(path = tk.path,
                              root = tk.root,
                              os = tk.os,
                              arch = tk.arch)
    else:
      self.__arch = arch
      self.__go = tk or which('go')
      self.__path = path
      self.__os = os
      self.__root = root
      self.__version = None
      self.__env = False
      self.__cxx_toolkit = cxx_toolkit
    if self.go is None:
      raise Exception('go executable is undefined. Check its installation')
    try:
      self.run(['help'])
    except FileNotFoundError:
      raise Exception('go executable not found')

  @property
  def arch(self):
    return self.__arch

  @property
  def go(self):
    return self.__go

  @property
  def path(self):
    if self.__path and not os.path.exists(self.__path):
      os.mkdir(self.__path)
    return self.__path

  @property
  def package_path(self):
    """
    Path used by `go get`.
    """
    return self.__path.split(':')[0]

  @property
  def os(self):
    return self.__os

  @property
  def root(self):
    return self.__root

  @property
  def env(self):
    """
    The Go specific environment for the toolkit.

    :return: The Go specific environment.
    :rtype: dict
    """
    if self.__env is not False:
      return self.__env
    self.__env = dict(os.environ)
    self.__env['CGO_ENABLED'] = '1'
    if self.__cxx_toolkit:
      self.__env['CC'] = self.__cxx_toolkit.c
      self.__env['CXX'] = self.__cxx_toolkit.cxx
    for k in Toolkit.properties:
      v = getattr(self, k)
      if v is not None:
        self.__env['GO%s' % k.upper()] = v
    return self.__env

  @property
  def host_env(self):
    exclude_env = ['GOARCH', 'GOOS']
    return dict((k, v) for k, v in self.env.items() if k not in exclude_env)

  def run(self, cmd, host = False):
    """
    Run the given command in the toolkit environment.

    :param cmd: Same as __run.
    :type cmd: Same as __run.

    :return: Same as __run
    :rtype: Same as __run
    """
    env = self.host_env if host else self.env
    return subprocess.check_output([self.go] + cmd,
                                   env = env).decode('utf-8').strip()

  def platform_str(self, host = False):
    vars = self.run(['env', 'GOOS', 'GOARCH'], host = host).splitlines()
    return '%s_%s' % (vars[0], vars[1])

  def exec_ext(self, host = False):
    tos = self.run(['env', 'GOOS'], host = host)
    if tos == 'windows':
      return '.exe'
    return ''

  def dylib_ext(self, host = False):
    tos = self.run(['env', 'GOOS'], host = host)
    ext = {
      'darwin': '.dylib',
      'linux': '.so',
      'windows': '.dll',
    }
    return ext.get(tos, '')

  def staticlib_ext(self, host = False):
    return '.a'

  def dependencies(self, node):
    """
    Return dependencies of the given go file.

    :arg node: The node.
    :type node: Source.

    :return: Dependencies from go list -json.
    :rtype: List of str
    """
    assert isinstance(node, Source)
    import json
    if not node.path().is_file():
      try:
        # FIXME: is building a node during dependencies ok ?
        # See drake.cxx._mkdeps for more detail.
        node.build()
      except drake.NoBuilder:
        # If a node is found but cannot be built, let drake fail by itself.
        pass
    res = json.loads(self.run(['list', '-json', str(node.path())]))
    deps = res['Deps']
    deps.extend(res.get('TestImports', []))
    return deps

  @property
  def version(self):
    """
    Return the version of Go used.

    :return: The version.
    :rtype: str.
    """
    import re
    if self.__version is None:
      self.__version = re.sub('^go version', '', self.run(['version'])).strip()
    return self.__version

  def hash(self):
    """
    The hash of the Toolkit used for dependency computation.

    :return: Hash of the Toolkit
    :rtype: str
    """
    return {
      'version': self.version,
    }

class Source(drake.Node):
  """
  A specific type of drake.Node representing a Go file.
  """

  def __init__(self, path):
    """
    Create a Source form its path.

    :param path: The path of the file.
    :type path: See drake.Node.__init__
    """
    super().__init__(path)

  def clone(self, path):
    return self.__class__(path)

# Assign ".go" files to Source automatically.
drake.Node.extensions['go'] = Source

class FetchPackage(drake.Builder):

  def package_target(url, toolkit, build_host = False):
    return drake.node('%s/pkg/%s/%s.a' %
        (toolkit.package_path, toolkit.platform_str(build_host), url))

  def __init__(self, url, toolkit, targets = [], build_host = False):
    self.__targets = targets if targets else [FetchPackage.package_target(
      url, toolkit, build_host)]
    self.__url = url
    self.__toolkit = toolkit
    self.__build_host = build_host
    super().__init__([], self.__targets)

  def command(self):
    # Check dependency that can be fetched, i.e.: is URL.
    if '/' not in self.__url or '.' not in self.__url.split('/')[0]:
      raise Exception('Unable to fetch dependency, %s not a URL' % self.__url)
    if not self.__toolkit.path:
      raise Exception('Automatic dependency fetching failed, '
                      'require a toolkit GOPATH')
    return [self.__toolkit.go, 'get', self.__url]

  def execute(self):
    env = self.__toolkit.host_env if self.__build_host else self.__toolkit.env
    cmd = self.command()
    res = self.cmd('Fetch %s' % self.__url, cmd, env = env)
    return res

  def hash(self):
    return {
      'toolkit': self.__toolkit.hash(),
    }

  def __str__(self):
    return 'Fetch Go package %s' % self.__url

class Builder(drake.Builder):
  """
  Builder to transform a Go source to an Executable using the given Toolkit and
  Config.

  Example:

  Builder(
    Source("main.go"),
    Toolkit(),
    Config(),
    Executable("bin/exe", create_builder = False));

  # or, simpler

  Executable(Source("main.go"), Toolkit(), Config())
  """
  def __init__(self, node, toolkit, config, target, dependencies,
               is_test = False, build_host = False):
    """
    Create a Builder from a Source, a Toolkit and a Config that creates
    a Executable.

    :param node: A Source.
    :type node: Source
    :param toolkit: The Toolkit to use.
    :type toolkit: Toolkit
    :param config: The configuration to use.
    :type config: Config
    :param target: The target Executable to be build
    :type target: Executable
    :param is_test: If the executable is a test
    :type is_test: bool
    """
    assert isinstance(node, Source)
    self.__source = node
    self.__target = target
    self.__toolkit = toolkit
    self.__config = config
    self.__dependencies = dependencies
    self.__url_dependencies = \
      list(filter(lambda d: '/' in d and '.' in d.split('/')[0],
                  toolkit.dependencies(node)))
    for d in self.__url_dependencies:
      target = FetchPackage.package_target(d, toolkit, build_host)
      if not target.builder:
        FetchPackage(d, toolkit, build_host = build_host)
      self.__dependencies.append(target)
    self.__is_test = is_test
    self.__build_host = build_host
    super().__init__(self.__dependencies + [node], [self.__target])

  def dependencies(self):
    return self.__dependencies + [self.__source]

  def command(self):
    """
    Return the command build for the given config, source and target.

    :return: The command to execute.
    :rtype: list of str.
    """
    res = [
      self.__toolkit.go,
      'test' if self.__is_test else 'build',
      '-o', str(self.__target.path()),
    ]
    if self.__is_test:
      res.append('-c')
    if len(self.__config.tags):
      res += [
        '-tags', ' '.join(self.__config.tags),
      ]
    if len(self.__config.ldflags):
      res += [
        '-ldflags', ' '.join(self.__config.ldflags),
      ]
    res.append(str(self.__source.path()))
    return res

  def execute(self):
    """
    Generate the target by fetching its dependencies and then running the build
    command with the correct local include path.

    :return: Whether the build was successful.
    :rtype: bool
    """
    env = self.__toolkit.host_env if self.__build_host else self.__toolkit.env
    cmd = self.command()
    # Add the include paths to GOPATH so that Go can find dependencies.
    if self.__config.include_paths:
      include_paths = list(self.__config.include_paths)
      if self.__toolkit.path:
        include_paths.insert(0, self.__toolkit.path)
      env['GOPATH'] = ':'.join(map(lambda p: os.path.abspath(str(p)),
                                   include_paths))
    return self.cmd('Generate %s' % self.__target,
                    cmd,
                    env = env)

  def hash(self):
    """
    Return an unique identifier for the builder.

    :return: An unique identifier
    :rtype: str
    """
    return {
      'toolkit': self.__toolkit.hash(),
      'config': self.__config.hash(),
    }

  def __str__(self):
    return 'Go build %s' % self.__target

class LibraryBuilder(drake.Builder):

  def __init__(self, node, toolkit, config, target, dependencies, lib_type,
               header = None, build_host = False):
    assert isinstance(node, Source)
    self.__source = node
    self.__target = target
    self.__toolkit = toolkit
    self.__config = config
    self.__dependencies = dependencies
    self.__url_dependencies = \
      list(filter(lambda d: '/' in d and '.' in d.split('/')[0],
                  toolkit.dependencies(node)))
    for d in self.__url_dependencies:
      t = FetchPackage.package_target(d, toolkit, build_host)
      if not t.builder:
        FetchPackage(d, toolkit, build_host = build_host)
      self.__dependencies.append(t)
    self.__type = lib_type
    self.__build_host = build_host
    targets = [self.__target]
    if header:
      targets.append(header)
    super().__init__(self.__dependencies + [node], targets)

  def command(self):
    res = [
      self.__toolkit.go,
      'build',
      '-buildmode=%s' % self.__type,
      '-o', str(self.__target),
    ]
    if len(self.__config.tags):
      res += ['-tags', ' '.join(self.__config.tags)]
    if len(self.__config.ldflags):
      res += ['-ldflags', ' '.join(self.__config.ldflags)]
    res.append(str(self.__source.path()))
    return res

  def execute(self):
    cmd = self.command()
    env = self.__toolkit.host_env if self.__build_host else self.__toolkit.env
    # Add the include paths to GOPATH so that Go can find dependencies.
    if self.__config.include_paths:
      include_paths = list(self.__config.include_paths)
      if self.__toolkit.path:
        include_paths.insert(0, self.__toolkit.path)
      env['GOPATH'] = ':'.join(map(lambda p: os.path.abspath(str(p)),
                                   include_paths))
    return self.cmd('Generate %s' % self.__target, cmd, env = env)

  def hash(self):
    return {
      'toolkit': self.__toolkit.hash(),
      'config': self.__config.hash(),
    }

  def __str__(self):
    return 'Go library %s' % self.__target

class Executable(drake.Node):
  """
  A specific type of drake.Node representing an executable build from a Go
  Source.
  By default, it creates the Builder to build itself.

  Example:

  Executable(Source("main.go"), Toolkit(), Config())
  """
  def __init__(self, source, toolkit = None, config = None, target = None,
               sources = [], create_builder = True, build_host = False):
    """
    Create an Executable from a given Source, Toolkit and Config.

    :param source: The source for the Executable
    :type source: Source
    :param toolkit: The Toolkit to use.
    :type toolkit: Toolkit
    :param config: The configuration to use.
    :type config: Config
    :param sources: list of sources the executable depends on
    :type sources: list of nodes
    :param create_builder: Whether if the Builder should be declared.
    :type create_builder: bool
    :param build_host: Build for host architecture and OS.
    :type build_host: bool
    """
    target = target or '%s%s' % (source.name().without_last_extension(),
                                 self.__toolkit.exec_ext(build_host))
    super().__init__(target)
    if create_builder:
      Builder(node = source, toolkit = toolkit, config = config, target = self,
              dependencies = sources, build_host = build_host)
    else:
      if toolkit:
        print("toolkit argument for %s was ignored" % self)
      if config:
        print("config argument for %s was ignored" % self)

class TestExecutable(drake.Node):

  def __init__(self, source, toolkit = None, config = None, target = None,
               sources = [], build_host = False):
    target = target or '%s%s' % (source.name().without_last_extension(),
                                 self.__toolkit.exec_ext(build_host))
    super().__init__(target)
    Builder(node = source, toolkit = toolkit, config = config, target = self,
            dependencies = sources, is_test = True, build_host = build_host)

class CDyLibBuilder(LibraryBuilder):

  def __init__(self, source, toolkit = None, config = None, target = None,
               sources = [], build_host = False):
    self.__library = drake.node(
      target or '%s%s' % (source.name().without_last_extension(),
                          toolkit.dylib_ext(build_host)))
    self.__header = drake.node('%s.h' % target.split('.')[0])
    super().__init__(
      node = source, toolkit = toolkit, config = config,
      target = self.__library, dependencies = sources, lib_type = 'c-shared',
      header = self.__header, build_host = build_host)

  @property
  def header(self):
    return self.__header

  @property
  def library(self):
    return self.__library

  def execute(self):
    if not super().execute():
      return False
    path = self.library.path()
    if '.dylib' in str(path.basename()):
      cmd = ['install_name_tool', '-id', '@rpath/%s' % path.basename(), path]
      return self.cmd('Set Go library ID %s' % self.library, cmd)
    else:
      return True

class CStaticLibBuilder(LibraryBuilder):

  def __init__(self, source, toolkit = None, config = None, target = None,
               sources = [], build_host = False):
    self.__library = drake.node(
      target or '%s%s' % (source.name().without_last_extension(),
                          toolkit.staticlib_ext(build_host)))
    self.__header = drake.node('%s.h' % target.split('.')[0])
    super().__init__(
      node = source, toolkit = toolkit, config = config,
      target = self.__library, dependencies = sources, lib_type = 'c-archive',
      header = self.__header, build_host = build_host)

  @property
  def header(self):
    return self.__header

  @property
  def library(self):
    return self.__library
