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
# - Explore dependencies
# - Fetch existing modules
# - Discover current architecture
# - Support all Go specific environment

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
    self.__ldflags = OrderedSet()
    self.__tags = OrderedSet()
    if config:
      self += config

  @property
  def ldflags(self):
    """
    Return the ldflags as a list.

    :return: The lisf of ldflags
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
      'tags': self.tags,
      'ldflags': self.ldflags,
    }


class Toolkit:
  """
  A Toolkit represents an abstraction of a Go environment.

  It'is designed to bind the GO* environment variables and offers a ways to get
  the go command to run or directly run them.
  """

  properties = ["home", "root", "os", "arch"]

  def __init__(self,
               tk = None,
               home = None,
               root = None,
               os = None,
               arch = None):
    """
    Create a toolkit or clone an existing one.

    :param tk: The toolkit to clone. If specified, other arguments must be None
    :type tk: Toolkit
    :param home: The home of your go environment (override GOHOME).
    :type home: str
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
      assert all(a is None for a in [home, root, os, arch])
      return super().__init__(home = tk.home,
                              root = tk.root,
                              os = tk.os,
                              arch = tk.arch)
    else:
      self.__arch = arch
      self.__go = tk or which('go')
      self.__home = home
      self.__os = os
      self.__root = root
      self.__version = None
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
  def home(self):
    return self.__home

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
    return {
      k: getattr(self, k)
      for k in Toolkit.properties
      if getattr(self, k) is not None
    }

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
    Return both Imports and Deps of the given go file.

    :arg node: The node.
    :type node: Source.

    :return: The tuple Imports, Deps from go list -json.
    :rtype: Tuple(list of str, list of str).
    """
    assert isinstance(node, Source)
    import json
    res = json.loads(self.run(['list', '-json', str(node.path())]))
    return res['Imports'], res['Deps']

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

  def list(self, entity):
    """
    Return the output of go list entity.

    :param entity: The path to the entity to list.
    :type entity: str, drake.Path, Source.
    """
    if isinstance(entity, Source):
      entity = entity.name()
    return self.run(['list', str(drake.path_source() / entity)])

  def command_build(self, config, source, target):
    """
    Return the command build for the given config, source and target.

    :param config: A configuration.
    :type config: Config
    :param source: A source file.
    :type source: Source
    :param target: The target to build.
    :type target: Executable

    :return: The command to execute.
    :rtype: list of str.
    """
    return [
      self.go,
      'build',
      '-o', str(target.path()),
      '-tags', ' '.join(config.tags),
      '-ldflags', ' '.join(config.ldflags),
      str(source.path()),
    ]

  def hash(self):
    """
    The hash of the Toolkit used for dependency computation.

    :return: Hash of the Toolkit
    :rtype: str
    """
    return {
      'version': self.version,
      'env': self.env,
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
  def __init__(self, node, toolkit, config, target):
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
    """
    assert isinstance(node, Source)
    self.__source = node
    self.__target = target
    self.__toolkit = toolkit
    self.__config = config
    super().__init__([node], [self.__target])
    if node.path().exists():
      self.compute_dependencies()

  def compute_dependencies(self):
    # TODO: Add depencencies to the builder.
    # self.__toolkit.dependencies(node)
    pass

  def execute(self):
    """
    Generate the target buy running the command.

    :return: Whether the build was successfull.
    :rtype: bool
    """
    cmd, env = self.__toolkit.command_build(self.__config,
                                            self.__source,
                                            self.__target)
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
      'toolkit': self.__toolkit.identifier(),
      'config': self.__config.identifier(),
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
               create_builder = True):
    """
    Create an Executable from a given Source, Toolkit and Config.

    :param source: The source for the Executable
    :type source: Source
    :param toolkit: The Toolkit to use.
    :type toolkit: Toolkit
    :param config: The configuration to use.
    :type config: Config
    :param create_builder: Whether if the Builder should be declared.
    :type create_builder: bool
    """
    target = target or source.name().without_last_extension()
    super().__init__(target)
    if create_builder:
      Builder(node = source, toolkit = toolkit, config = config, target = self)
    else:
      if toolkit:
        print("toolkit argument for %s was ignored" % self)
      if config:
        print("config argument for %s was ignored" % self)
