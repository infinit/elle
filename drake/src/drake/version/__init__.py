import drake
import drake.git
import collections
import itertools

Version = drake.Version

class VersionGenerator(drake.Builder):

  def __init__(self, output, git = None, production_build = True):
    git = git or drake.git.Git()
    drake.Builder.__init__(self, [git], [output])
    self.__git = git
    self.__output = output
    self.__production_build = production_build

  def execute(self):
    self.output('Generate %s' % self.__output.path())
    chunks = collections.OrderedDict()
    if self.__production_build:
      version = self.__git.description()
    else:
      version = '%s-dev' % self.__git.version().split('-')[0]
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

  def hash(self):
    return self.__production_build
