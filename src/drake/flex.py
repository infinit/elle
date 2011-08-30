# Copyright (C) 2011, Quentin "mefyl" Hocquet
#
# This software is provided "as is" without warranty of any kind,
# either expressed or implied, including but not limited to the
# implied warranties of fitness for a particular purpose.
#
# See the LICENSE file for more information.

import drake
import sys

class SDK(drake.Configuration):

    """Configuration for the Flex SDK library."""

    def __init__(self, prefix = None):
        """Find and create a configuration for the Flex SDK.

        prefix -- Where to find the Flex SDK, should contain bin/acompc.
        """
        # Compute the search path.
        if prefix is None:
            test = [drake.Path('/usr'), drake.Path('/usr/local')]
        else:
            test = [drake.Path(prefix)]
        self.__prefix = self._search_all('bin/acompc', test)[0]
        self.__options = []
        self.__external_library_path = []

    def add_option(self, option):
        self.__options.append(option)

    def flags(self):
        def mkpath(path):
          if not path.absolute():
            path = drake.srctree() / path
          return str(path)
        options = ''.join(map(lambda o: ' %s' % o, self.__options))
        external_libraries = ''
        if self.__external_library_path:
            external_libraries = ':'.join(map(mkpath, self.__external_library_path))
            external_libraries = ' -external-library-path+=%s' % external_libraries
        return options + external_libraries

    def add_external_library_path(self, path):
        path = drake.Path(path)
        self.__external_library_path.append(path)

    def acompc(self):
        return self.__prefix / 'bin/acompc'

    def mxmlc(self):
        return self.__prefix / 'bin/mxmlc'

    def __repr__(self):
        return 'SofiaSIP(prefix = %s)' % repr(self.__prefix)


class Source(drake.Node):

  pass

drake.Node.extensions['as'] = Source


class Library(drake.Node):

  def __init__(self, name, sources, flex_sdk):
    self.__sources = sources
    self.__sdk = flex_sdk
    drake.Node.__init__(self, name)
    LibraryBuilder(sources, self, flex_sdk)

drake.Node.extensions['swc'] = Library


class LibraryBuilder(drake.Builder):

  def __init__(self, sources, target, sdk):
    self.__sources = list(filter(lambda s: not isinstance(s, Library), sources))
    self.__libraries = list(filter(lambda s: isinstance(s, Library), sources))
    self.__target = target
    self.__sdk = sdk
    drake.Builder.__init__(self, sources, [target])

  def execute(self):
    # Options
    options = ' -compiler.locale -compiler.optimize -static-link-runtime-shared-libraries -compiler.debug'
    # Sources
    sources = ' '.join(map(lambda src: str(src.path()), self.__sources))
    # Libraries
    libraries = ''
    if self.__libraries:
      libraries = ' -external-library-path+=%s' % ':'.join(map(lambda n: str(n.path().dirname()), self.__libraries))
    # Output
    output = str(self.__target.path())
    # Command
    return self.cmd('Flex %s' % self.__target, '%s%s %s -include-sources %s%s -output %s', str(self.__sdk.acompc()), self.__sdk.flags(), options, sources, libraries, output)
#    _local/flex_sdk/bin/acompc -external-library-path+=modules/bindings/flash/UObjectFlash/libs/ -compiler.locale -compiler.optimize -static-link-runtime-shared-libraries -compiler.debug -keep-as3-metadata Urbi -include-sources modules/bindings/flash/UObjectFlash/com/ -output /tmp/UObjeftFlash.swc


class Application(drake.Node):

  def __init__(self, name, config, source_path, source, sources, flex_sdk):
    source_path = drake.Path(source_path)
    self.__config = config
    self.__source = source
    self.__source_path = source_path
    self.__sources = sources
    self.__sdk = flex_sdk
    drake.Node.__init__(self, name)
    ApplicationBuilder(sources, self, config, source, source_path, flex_sdk)

drake.Node.extensions['swf'] = Application


class ApplicationBuilder(drake.Builder):

  def __init__(self, sources, target, config, source, source_path, flex_sdk):
    self.__config = config
    self.__source_path = source_path
    self.__source = source
    self.__sources = list(filter(lambda s: not isinstance(s, Library), sources))
    self.__libraries = list(filter(lambda s: isinstance(s, Library), sources))
    self.__target = target
    self.__sdk = flex_sdk
    drake.Builder.__init__(self, sources, [target])

  def execute(self):
    # Options
    options = ' -compiler.locale -compiler.optimize -static-link-runtime-shared-libraries -compiler.debug'
    # Sources
    sources = ' '.join(map(lambda src: str(src.path()), self.__sources))
    # Libraries
    libraries = ''
    if self.__libraries:
      libraries = ' -external-library-path+=%s' % ':'.join(map(lambda n: str(n.path().dirname()), self.__libraries))
    # Output
    output = str(self.__target.path())
    # Command
    return self.cmd('Flex %s' % self.__target,
                    '%s%s -load-config %s -compiler.source-path %s -output %s %s',
                    self.__sdk.mxmlc(),
                    libraries,
                    self.__config.path(),
                    drake.srctree() / self.__source_path,
                    self.__target.path(),
                    self.__source.path(),
                    )

# $PWD/_local/flex_sdk/bin/mxmlc -compiler.source-path submodules/modules/bindings/flash/FlashServer  submodules/modules/bindings/flash/FlashServer/FlashServer.mxml -compiler.external-library-path+=_build/submodules/modules/bindings/flash/UObjectFlash:_build/submodules/modules/bindings/flash/UStdLib
