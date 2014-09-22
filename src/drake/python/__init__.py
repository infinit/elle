# Copyright (C) 2014, Quentin "mefyl" Hocquet
#
# This software is provided "as is" without warranty of any kind,
# either expressed or implied, including but not limited to the
# implied warranties of fitness for a particular purpose.
#
# See the LICENSE file for more information.

import drake

class Package(drake.VirtualNode):

  '''A set of nodes constituting a python package making it easy to
  depend on the whole package or get the pythonpath.'''

  def __init__(self, name, root, nodes):
    '''Create a python Package

    name  -- the node name
    root  -- the package root
    nodes -- the package files
    '''
    super().__init__(name)
    self.__root_source = drake.path_source(root)
    self.__root_build = drake.path_build(root)
    self.__nodes = nodes
    for node in nodes:
      self.dependency_add(node)

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
