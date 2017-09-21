# Copyright (C) 2014, Quentin "mefyl" Hocquet
#
# This software is provided "as is" without warranty of any kind,
# either expressed or implied, including but not limited to the
# implied warranties of fitness for a particular purpose.
#
# See the LICENSE file for more information.

import drake
import itertools
import drake.cxx

class Package(drake.VirtualNode):

  '''A set of nodes constituting a python package making it easy to
  depend on the whole package or get the pythonpath.'''

  def __init__(self, name, root, nodes, fullname = None):
    '''Create a python Package

    name  -- the node name
    root  -- the package root
    nodes -- the package files
    '''
    super().__init__(name)
    self.__root = root
    self.__root_source = drake.path_source(root)
    self.__root_build = drake.path_build(root)
    self.__nodes = nodes
    for node in nodes:
      self.dependency_add(node)
    self.__fullname = fullname or name

  @property
  def pythonpath(self):
    '''A list of path to add to the pythonpath to find this module.

    >>> with drake.Drake('srcdir'):
    ...   pkg = drake.python.Package(
    ...     'somepkg', 'path/to', drake.nodes('path/to/__init__.py'))
    ...   pkg.pythonpath
    [Path("srcdir/path/to"), Path("path/to")]
    '''
    return [self.__root_source, self.__root_build]

  @property
  def root(self):
    return self.__root

  @property
  def nodes(self):
    return list(self.__nodes)

  @property
  def fullname(self):
    return self.__fullname

def find(python = None,
         version = None):
  if version is None:
    versions = (drake.Version(3, 6),
                drake.Version(3, 5),
                drake.Version(3, 4),
                drake.Version(3, 3),
                drake.Version(3, 2))
  elif not isinstance(version, collections.Iterable):
    versions = (version,)
  else: # version is already a list.
    versions = version
  tk = drake.cxx.Toolkit()
  windows = tk.os is drake.os.windows
  if python is None and drake.cxx.PkgConfig.available:
    def options():
      for v in versions:
        yield ('python', v)
        yield ('python3', v) # CentOS
        yield ('python-%s' % v, v) # Gentoo
    for pkg_name, version in options():
      pkg = drake.cxx.PkgConfig(pkg_name, version = version)
      if pkg.exists:
        python3 = pkg.prefix
        break
  include_dir = list(itertools.chain(
    ('include',),
    *(('include/python%s' % v,
       'include/python%sm' % v) for v in versions)))
  python = drake.cxx.find_library(
    'pyconfig.h',
    prefix = python,
    include_dir = include_dir)
  python_version = tk.preprocess('''\
#include <patchlevel.h>
PY_MAJOR_VERSION
PY_MINOR_VERSION''', config = python)
  python_version = python_version.split('\n')[-3:-1]
  python_version = drake.Version(*map(int, python_version))
  if windows:
    python_bin = 'python.exe'
  else:
    python_bin = 'bin/python%s' % python_version
  python.python_interpreter = python.prefix / python_bin
  python.version = python_version
  return python
