# Copyright (C) 2013-2016, Quentin "mefyl" Hocquet
#
# This software is provided "as is" without warranty of any kind,
# either expressed or implied, including but not limited to the
# implied warranties of fitness for a particular purpose.
#
# See the LICENSE file for more information.

import drake

class Context:
  """The context holds the nsis script resources.
  """
  current = None

  def __init__(self,
               target = None,
               resources = []):
    self.resources = resources
    self.target = target

  def __enter__(self):
    self.__previous = Context.current
    Context.current = self

  def __exit__(self, *args, **kwargs):
    Context.current = self.__previous

class Script(drake.Node):
  """The nsis script file.
  """

  def __init__(self, path, resources = [], *args, **kwargs):
    super().__init__(path = path, *args, **kwargs)
    self.__resources = resources
    context = Context.current
    if context is not None:
      Compiler(self,
               target = context.target,
               resources = context.resources)

  @property
  def resources(self):
    return self.__resources

drake.Node.extensions['nsi'] = Script

class Compiler(drake.Builder):
  """Compile the nsis script to an installer.
  """
  def __init__(self,
               script,
               target = None,
               resources = [],
               leave_stdout = False):
    self.__script = script
    resources = resources or self.__script.resources
    target = target or script.name().with_extension('exe')
    self.__target = target
    self.__leave_stdout = leave_stdout
    super().__init__([self.__script] + resources,
                     [self.__target])

  def execute(self):
    return self.cmd('Compile %s' % self.__target,
                    self.command,
                    leave_stdout = self.__leave_stdout)

  @property
  def command(self):
    return ['makensis',
            '-NOCD',
            '-XOutFile %s' % drake.path_build(self.__target.name()),
            self.__script]

  @property
  def rendered(self):
    return self.__target
