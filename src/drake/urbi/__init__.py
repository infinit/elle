import drake
import drake.cxx

class SDK:

  def __init__(self, root = None):

    if root is None:
      paths = [drake.Path('/usr'), drake.Path('/usr/local')]
    else:
      paths = [drake.Path(root)]
    self.__root = self.__search('bin/urbi', paths)
    self.__config = drake.cxx.Config()
    self.__config.add_system_include_path(self.__root / 'include')
    self.__config.lib('boost_system')

  def config(self):

    return self.__config

  def __search(self, what, where):

    what = drake.Path(what)
    for root in where:
      if (root / what).exists():
        return root
    if len(where) <= 1:
      desc = str(where[0])
    else:
      desc = '%s and %s' % (', '.join(map(str, where[:-1])), where[-1])
    raise drake.Exception('Unable to find %s in any of %s.' % (what, desc))

  def urbi(self):
    return self.__root / 'bin/urbi'

class UObject(drake.cxx.Module):

  def __init__(self, name, sources,
               urbi_sdk = None,
               cxx_toolkit = None,
               cxx_config = None):

    urbi_sdk = urbi_sdk or drake.urbi.SDK()
    cxx_toolkit = cxx_toolkit or drake.cxx.Toolkit()
    cxx_config = cxx_config or drake.cxx.Config()
    drake.cxx.Module.__init__(self, name, sources,
                              cxx_toolkit, cxx_config + urbi_sdk.config())
