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
               arch = None):
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
    for k in Toolkit.properties:
      v = getattr(self, k)
      if v is not None:
        self.__env['GO%s' % k.upper()] = v
    return self.__env

  def run(self, cmd):
    """
    Run the given command in the toolkit environment.

    :param cmd: Same as __run.
    :type cmd: Same as __run.

    :return: Same as __run
    :rtype: Same as __run
    """
    return subprocess.check_output([self.go] + cmd,
                                   env = self.env).decode('utf-8').strip()

  def dependencies(self, node):
    """
    Return dependencies of the given go file.

    :arg node: The node.
    :type node: Source.

    :return: Imports and missing dependencies from go list -json.
    :rtype: Tuple list of str
    """
    assert isinstance(node, Source)
    import json
    res = json.loads(self.run(['list', '-json', str(node.path())]))
    imports = set()
    missing_deps = set()
    test_imports = res.get('TestImports', [])
    for i in res.get('Imports', []):
      imports.add(i)
    for e in res.get('DepsErrors', []):
      missing_deps.add(e['ImportStack'][-1])
    # `go list` has a different output for tests and normal files.
    # In the case of tests, we have to return all imports as missing deps.
    imports.update(test_imports)
    missing_deps.update(test_imports)
    return list(imports), list(missing_deps)

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

  def command_fetch_deps(self, deps):
    # Filter for dependencies that can be fetched, i.e.: URLs.
    url_deps = list(filter(lambda d: '/' in d and '.' in d.split('/')[0], deps))
    if len(url_deps) == 0:
      return None
    if len(url_deps) > 0 and not self.path:
      raise Exception('Automatic dependency fetching failed, '
                      'require a toolkit GOPATH')
    # `go get -u` only fetches missing dependencies and will not update existing
    return [self.go, 'get', '-u'] + url_deps

  def command_build(self, config, source, target, is_test):
    """
    Return the command build for the given config, source and target.

    :param config: A configuration.
    :type config: Config
    :param source: A source file.
    :type source: Source
    :param target: The target to build.
    :type target: Executable
    :param is_test: If the executable is a test
    :type is_test: bool

    :return: The command to execute.
    :rtype: list of str.
    """
    res = [
      self.go,
      'test' if is_test else 'build',
      '-o', str(target.path()),
    ]
    if is_test:
      res.append('-c')
    if len(config.tags):
      res += [
        '-tags', ' '.join(config.tags),
      ]
    if len(config.ldflags):
      res += [
        '-ldflags', ' '.join(config.ldflags),
      ]
    res.append(str(source.path()))
    return res

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
               is_test = False):
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
    self.__imports = set()
    self.__dependencies = dependencies
    self.__missing_dependencies = set()
    self.__is_test = is_test
    super().__init__(self.__dependencies + [node], [self.__target])

  def dependencies(self):
    return self.__dependencies + [self.__source]

  def compute_dependencies(self):
    # Add depencencies to the builder.
    for node in self.dependencies():
      if isinstance(node, Source):
        res = self.__toolkit.dependencies(node)
        self.__imports.update(res[0])
        self.__missing_dependencies.update(res[1])

  def execute(self):
    """
    Generate the target by fetching its dependencies and then running the build
    command with the correct local include path.

    :return: Whether the build was successful.
    :rtype: bool
    """
    self.compute_dependencies()
    cmd_deps = self.__toolkit.command_fetch_deps(self.__missing_dependencies)
    if cmd_deps:
      deps_status = self.cmd('Check Go dependencies %s' % self.__target,
                             cmd_deps,
                             env = self.__toolkit.env)
      if not deps_status:
        return deps_status
    cmd = self.__toolkit.command_build(self.__config,
                                       self.__source,
                                       self.__target,
                                       self.__is_test)
    env = self.__toolkit.env
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

class Executable(drake.Node):
  """
  A specific type of drake.Node representing an executable build from a Go
  Source.
  By default, it creates the Builder to build itself.

  Example:

  Executable(Source("main.go"), Toolkit(), Config())
  """
  def __init__(self, source, toolkit = None, config = None, target = None,
               sources = [], create_builder = True):
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
    """
    target = target or source.name().without_last_extension()
    super().__init__(target)
    if create_builder:
      Builder(node = source, toolkit = toolkit, config = config, target = self,
              dependencies = sources)
    else:
      if toolkit:
        print("toolkit argument for %s was ignored" % self)
      if config:
        print("config argument for %s was ignored" % self)

class TestExecutable(drake.Node):

  def __init__(self, source, toolkit = None, config = None, target = None,
               sources = []):
    target = target or source.name().without_last_extension()
    super().__init__(target)
    Builder(node = source, toolkit = toolkit, config = config, target = self,
            dependencies = sources, is_test = True)

class GoPackage(drake.Node):

  class Fetch(drake.Builder):

    def __init__(self, package, toolkit, target):
      self.__package = package
      self.__toolkit = toolkit
      self.__target = target
      super().__init__([], [self.__target])

    def execute(self):
      cmd = self.__toolkit.command_fetch_deps([self.__package])
      return self.cmd('Fetch %s' % self.__target, cmd, env = self.__toolkit.env)

    def hash(self):
      return {
        'toolkit': self.__toolkit.hash(),
      }

  def __init__(self, package, target, toolkit = None):
    super().__init__(target)
    GoPackage.Fetch(package = package, toolkit = toolkit, target = self)
