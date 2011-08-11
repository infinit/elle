import drake
import drake.cxx

class SDK(drake.Configuration):

  def __init__(self, prefix = None, version = drake.Version()):
    if prefix is None:
      paths = [drake.Path('/usr'), drake.Path('/usr/local')]
    else:
      paths = [drake.Path(prefix)]
    self.__prefix = self._search('bin/urbi', paths)
    v = drake.cmd_output([str(self.urbi()), '--quiet', '--expression', 'System.version;shutdown;'])
    v = v.decode('utf-8').split(' ')[1].strip().split('"')[1].split('.')
    v = drake.Version(*map(int, v))
    if v not in version:
      raise Exception('Urbi SDK version (%s) does not match the requested version (%s).' % (v, version))
    self.__config = drake.cxx.Config()
    self.__config.add_system_include_path(self.__prefix / 'include')
    self.__config.lib('boost_system')

  def config(self):

    return self.__config

  def urbi(self):
    return self.__prefix / 'bin/urbi'

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
