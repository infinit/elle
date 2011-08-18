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

  def runtime_nodes(self):
    res = []
    res.append('bin/urbi')
    res.append('lib/gostai/engine/libuobject.so')
    res.append('lib/gostai/remote/libuobject.so')
    res.append('lib/gostai/uobjects/urbi/fusion.so')
    res.append('lib/gostai/uobjects/urbi/logger.so')
    res.append('lib/gostai/uobjects/urbi/md5.so')
    res.append('lib/gostai/uobjects/urbi/process.so')
    res.append('lib/gostai/uobjects/urbi/regexp.so')
    res.append('lib/gostai/uobjects/urbi/rtp.so')
    res.append('lib/gostai/uobjects/urbi/stream.so')
    res.append('lib/libjpeg4urbi.so')
    res.append('lib/libport.so')
    res.append('lib/libsched.so')
    res.append('lib/libserialize.so')
    res.append('lib/liburbi.so')
    res.append('lib/liburbijava.so')
    res.append('share/gostai/urbi/binary.u')
    res.append('share/gostai/urbi/boolean.u')
    res.append('share/gostai/urbi/call-message.u')
    res.append('share/gostai/urbi/channel.u')
    res.append('share/gostai/urbi/code.u')
    res.append('share/gostai/urbi/comparable.u')
    res.append('share/gostai/urbi/component.u')
    res.append('share/gostai/urbi/container.u')
    res.append('share/gostai/urbi/control.u')
    res.append('share/gostai/urbi/dataflow.u')
    res.append('share/gostai/urbi/date.u')
    res.append('share/gostai/urbi/dictionary.u')
    res.append('share/gostai/urbi/directory.u')
    res.append('share/gostai/urbi/duration.u')
    res.append('share/gostai/urbi/enumeration.u')
    res.append('share/gostai/urbi/event.u')
    res.append('share/gostai/urbi/exception.u')
    res.append('share/gostai/urbi/file.u')
    res.append('share/gostai/urbi/float.u')
    res.append('share/gostai/urbi/formatter.u')
    res.append('share/gostai/urbi/global.u')
    res.append('share/gostai/urbi/group.u')
    res.append('share/gostai/urbi/image.u')
    res.append('share/gostai/urbi/input-stream.u')
    res.append('share/gostai/urbi/job.u')
    res.append('share/gostai/urbi/kernel1.u')
    res.append('share/gostai/urbi/lazy.u')
    res.append('share/gostai/urbi/list.u')
    res.append('share/gostai/urbi/loadable.u')
    res.append('share/gostai/urbi/lobby.u')
    res.append('share/gostai/urbi/logger.u')
    res.append('share/gostai/urbi/math.u')
    res.append('share/gostai/urbi/mutex.u')
    res.append('share/gostai/urbi/naming-standard.u')
    res.append('share/gostai/urbi/nil.u')
    res.append('share/gostai/urbi/object.u')
    res.append('share/gostai/urbi/orderable.u')
    res.append('share/gostai/urbi/package-info.u')
    res.append('share/gostai/urbi/package-info/libport.u')
    res.append('share/gostai/urbi/package-info/urbi-sdk-remote.u')
    res.append('share/gostai/urbi/package-info/urbi-sdk.u')
    res.append('share/gostai/urbi/path.u')
    res.append('share/gostai/urbi/pattern.u')
    res.append('share/gostai/urbi/platform.u')
    res.append('share/gostai/urbi/process.u')
    res.append('share/gostai/urbi/profile.u')
    res.append('share/gostai/urbi/profiling.u')
    res.append('share/gostai/urbi/pubsub.u')
    res.append('share/gostai/urbi/python.u')
    res.append('share/gostai/urbi/range-iterable.u')
    res.append('share/gostai/urbi/regexp.u')
    res.append('share/gostai/urbi/require-file.u')
    res.append('share/gostai/urbi/semaphore.u')
    res.append('share/gostai/urbi/singleton.u')
    res.append('share/gostai/urbi/socket.u')
    res.append('share/gostai/urbi/stack-frame.u')
    res.append('share/gostai/urbi/string.u')
    res.append('share/gostai/urbi/system.u')
    res.append('share/gostai/urbi/tag.u')
    res.append('share/gostai/urbi/test-suite.u')
    res.append('share/gostai/urbi/test.u')
    res.append('share/gostai/urbi/timeout.u')
    res.append('share/gostai/urbi/traceable.u')
    res.append('share/gostai/urbi/trajectory-generator.u')
    res.append('share/gostai/urbi/tuple.u')
    res.append('share/gostai/urbi/uobject.u')
    res.append('share/gostai/urbi/updatehook-stack.u')
    res.append('share/gostai/urbi/urbi.u')
    res.append('share/gostai/urbi/utags.u')
    res.append('share/gostai/urbi/vector.u')
    res.append('share/gostai/urbi/weak-pointer.u')
    return list(map(lambda p: drake.Node('%s/%s' % (self.prefix(), p)), res))

  def prefix(self):
    return self.__prefix

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
