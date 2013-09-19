import drake
import os
import subprocess

class Valgrind:

  def __init__(self, path = None):
    if path is None:
      path = drake.Path('valgrind')
    elif isinstance(path, Valgrind):
      self.__path = drake.Path(path.__path)
      self.__version = drake.Version(path.__version)
      return
    self.__path = drake.Path(path)
    try:
      output = subprocess.check_output([str(self.path), '--version'])
    except Exception as e:
      raise Exception('Unable to find %s' % self.path) from e
    output = output.decode()
    prefix = 'valgrind-'
    if not output.startswith(prefix):
      raise Exception('Unable to parse valgrind version: %s' % output)
    output = output[len(prefix):]
    self.__version = drake.Version(output)

  @property
  def path(self):
    return self.__path

  @property
  def version(self):
    return self.__version


class ValgrindRunner(drake.Runner):

  def __init__(self, exe, args = None, env = None, valgrind = None):
    super().__init__(exe, args = args, env = env)
    self.__valgrind = Valgrind(valgrind)
    self.__valgrind_status = drake.node(
      '%s.valgrind' % self.executable.name())
    self.__valgrind_status.builder = self

  @property
  def command(self):
    return [
      str(self.__valgrind.path),
      '--leak-check=full',
      '--num-callers=50',
      '--log-file=%s' % self.__valgrind_status.path(),
      '--error-exitcode=1',
    ] + super().command
