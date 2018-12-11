import collections
import itertools
import subprocess

import drake
import drake.git

class FatLibraryGenerator(drake.Builder):

  def __init__(self,
               input_libs,
               output_lib,
               headers = [],
               input_headers = None,
               output_headers = None):
    drake.Builder.__init__(self,
                           input_libs,
                           itertools.chain([output_lib], (drake.node(output_headers / p)
                                                for p in headers)))
    self.__input_libs = input_libs
    self.__output_lib = output_lib
    self.__headers = headers
    if input_headers:
      self.__input_headers = drake.path_build(input_headers)
    else:
      self.__input_headers = None
    if output_headers:
      self.__output_headers = drake.path_build(output_headers)
    else:
      self.__output_headers = None

  def execute(self):
    res = self.cmd('Lipo %s' % self.input_paths,
                   self.lipo_command,
                   leave_stdout = False)
    if not res:
      return False
    if self.__headers and self.__input_headers and self.__output_headers:
      res = self.cmd('cp %s' % self.__input_headers,
                     self.copy_headers_command,
                     leave_stdout = False)
    return res

  @property
  def lipo_command(self):
    if len(self.__input_libs) == 1:
      res = ['cp']
      res.extend(self.input_paths)
      res.append(self.__output_lib.path())
    else:
      res = ['lipo']
      res.extend(self.input_paths)
      res.extend(['-create', '-output'])
      res.append(self.__output_lib.path())
    return res

  @property
  def input_paths(self):
    res = []
    for input in self.__input_libs:
      res.append(input.path())
    return res

  @property
  def copy_headers_command(self):
    return ['cp', '-r',
            self.__input_headers, self.__output_headers]


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


class PythonVersionGenerator(VersionGenerator):

  def _variable(self, name, value):
    return '%s = %s' % (name, repr(value))

class CxxVersionGenerator(VersionGenerator):

  def __init__(self, prefix, *args, **kwargs):
    super().__init__(*args, **kwargs)
    self.__prefix = prefix

  def _variable(self, name, value):
    try:
      return '#define %s_%s %s' % \
        (self.__prefix.upper(), name.upper(), int(value))
    except:
      return '#define %s_%s "%s"' % \
        (self.__prefix.upper(), name.upper(), value)

  def _prologue(self):
    yield '#ifndef %s_GIT_VERSION_HH' % self.__prefix
    yield '# define %s_GIT_VERSION_HH' % self.__prefix
    yield ''

  def _epilogue(self):
    yield ''
    yield '#endif'


def set_local_libcxx(cxx_toolkit):
  def _set_local_libcxx(tgt):
    if cxx_toolkit.os in [drake.os.macos]:
      with drake.WritePermissions(drake.node(tgt)):
        return drake.command([
          'install_name_tool', '-change',
          '/usr/lib/libc++.1.dylib', '@rpath/libc++.1.dylib', str(drake.path_build(tgt, True))
          ])
    else:
      return True
  return _set_local_libcxx

class Keychain():

  def __init__(self, keychain_path, keychain_password):
    self.__keychain = str(keychain_path)
    self.__keychain_password = keychain_password

  def _unlock_keychain(self):
    output = subprocess.check_output(
      ['security', 'list-keychains']).decode('utf-8').split('\n')
    found = False
    existing_keychains = []
    for keychain in output:
      # Don't want to re-add the system keychain.
      if len(keychain.strip()) > 0:
        if keychain.strip(' "') != '/Library/Keychains/System.keychain':
          existing_keychains.append(keychain.strip(' "'))
      if keychain.strip(' "') == self.__keychain:
        found = True
    if not found:
      args = ['security', 'list-keychains', '-s']
      args.extend(existing_keychains)
      args.append(self.__keychain)
      subprocess.check_call(args)
    subprocess.check_call(['security', 'unlock-keychain', '-p',
                           self.__keychain_password, self.__keychain])

  def _lock_keychain(self):
    subprocess.check_call(['security', 'lock-keychain', self.__keychain])

  def __enter__(self):
    self._unlock_keychain()

  def __exit__(self, *args):
    self._lock_keychain()
