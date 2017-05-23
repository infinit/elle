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

  def command_fetch_dep(self, dep):
    # Check dependency that can be fetched, i.e.: is URL.
    if '/' not in dep or '.' not in dep.split('/')[0]:
      raise Exception('Unable to fetch dependency, %s is not a URL' % dep)
    if not self.path:
      raise Exception('Automatic dependency fetching failed, '
                      'require a toolkit GOPATH')
    return [self.go, 'get', dep]

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

  def execute(self):
    env = self.__toolkit.host_env if self.__build_host else self.__toolkit.env
    cmd = self.__toolkit.command_fetch_dep(self.__url)
    res = self.cmd('Fetch %s' % self.__url, cmd, env = env)
    return res

  def hash(self):
    return {
      'toolkit': self.__toolkit.hash(),
    }

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

  def execute(self):
    """
    Generate the target by fetching its dependencies and then running the build
    command with the correct local include path.

    :return: Whether the build was successful.
    :rtype: bool
    """
    cmd = self.__toolkit.command_build(self.__config,
                                       self.__source,
                                       self.__target,
                                       self.__is_test)
    env = self.__toolkit.host_env if self.__build_host else self.__toolkit.env
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
    target = target or source.name().without_last_extension()
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
    target = target or source.name().without_last_extension()
    super().__init__(target)
    Builder(node = source, toolkit = toolkit, config = config, target = self,
            dependencies = sources, is_test = True, build_host = build_host)
