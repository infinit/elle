# Copyright (C) 2009-2011, Quentin "mefyl" Hocquet
#
# This software is provided "as is" without warranty of any kind,
# either expressed or implied, including but not limited to the
# implied warranties of fitness for a particular purpose.
#
# See the LICENSE file for more information.

import drake
import platform
import re
from .. import Builder, Exception, Node, Path, node, _CACHEDIR, debug
from .  import Config, StaticLib, Header, Object, Source

#class MocBuilder:

deps_handler_name = 'drake.cxx.qt.moc'

class Qt:

  def __init__(self, prefix = None, gui = False):
    self.files = {}
    if prefix is None:
      test = ['/usr', '/usr/local']
    else:
      test = [prefix]
    beacon = Path('Qt/qconfig.h')
    subdirs = [Path('.'), Path('qt4')]
    lib_suffix = ''
    if platform.system() == 'Windows':
      lib_suffix = '4'
    for path in test:
      p = Path(path)
      if not p.absolute:
        p = drake.path_source() / p
      for subdir in subdirs:
        if (p / 'include' / subdir / beacon).exists():
          self.prefix = Path(path)
          self.__cfg = Config()
          self.__cfg.add_system_include_path(self.prefix / 'include' / subdir)
          self.__cfg.add_system_include_path(self.prefix / 'include' / subdir / 'Qt')
          self.__cfg.add_system_include_path(self.prefix / 'include' / subdir / 'QtCore')
          self.__cfg.lib_path(self.prefix / 'lib' / subdir)
          self.__cfg.lib('QtCore%s' % lib_suffix)
          self.__cfg_gui = Config()
          self.__cfg_gui.add_system_include_path(self.prefix / 'include' / subdir / 'QtGui')
          self.__cfg.lib('QtGui%s' % lib_suffix)
          self.__cfg_xml = Config()
          self.__cfg_xml.add_system_include_path(self.prefix / 'include' / subdir / 'QtXml')
          self.__cfg.lib('QtXml%s' % lib_suffix)
          self.__cfg_network = Config()
          self.__cfg_network.add_system_include_path(self.prefix / 'include' / subdir / 'QtNetwork')
          self.__cfg.lib('QtNetwork%s' % lib_suffix)
          self.__cfg_webkit = Config()
          self.__cfg_webkit.add_system_include_path(self.prefix / 'include' / subdir / 'QtWebKit')
          self.__cfg.lib('QtWebKit%s' % lib_suffix)
          return
    raise Exception('unable to find %s in %s' % (beacon, ', '.join(test)))

  @property
  def config(self):
    return self.__cfg

  @property
  def config_gui(self):
    return self.__cfg_gui

  @property
  def config_xml(self):
    return self.__cfg_xml

  @property
  def config_network(self):
    return self.__cfg_network

  @property
  def config_webkit(self):
    return self.__cfg_webkit

  def plug(self, tk):

      tk.hook_bin_deps_add(self.hook_bin_deps)
      tk.hook_bin_src_add(self.hook_bin_src)
      tk.qt = self

  moc_re = re.compile('Q_OBJECT')

  # Find any header in our dependency that contains a Q_OBJECT, run
  # it through the Moc and add the result to our sources.
  def hook_bin_deps(self, linker, current = None):

      def find_moc_sources(builder, take, reject, where = True):
          for source in list(builder.sources().values()) + list(builder.sources_dynamic()):
              with debug.indentation():
                  if isinstance(source, StaticLib):
                      find_moc_sources(source.builder, take, reject, False)
                      continue
                  if isinstance(source, Header):
                      found = False
                      for line in open(str(source.path()), 'rb'):
                          line = line.decode('utf-8')
                          if re.search(self.moc_re, line):
                              found = True
                              break
                      if found:
                          if where:
                              take.add(source)
                          else:
                              reject.add(source)
                  if source.builder is not None:
                      find_moc_sources(source.builder, take, reject, where)

      take = set()
      reject = set()
      find_moc_sources(linker, take, reject)
      for source in take - reject:
          o = self.moc_file(linker, source)
          linker.add_dynsrc(deps_handler_name, o)

  def hook_bin_src(self, src):

      if isinstance(src, Ui):
          p = Path(src.name())
          p.extension = '%s.hh' % p.extension
          res = Header(p)
          Uic(self, src, res)
          return res

  def moc_file(self, linker, header):
      path = Path(header.name()).with_extension('moc.cc')
      src = node(path)
      if src.builder is None:
          Moc(self, header, src)
      return Object(src, linker.toolkit, linker.config)


def deps_handler(builder, path_obj, t, data):

    if path_obj in drake.Drake.current.nodes:
        return node(path_obj)
    path_src = Path(path_obj).with_extension('moc.cc')
    src = node(path_src)
    path_header = Path(path_obj).with_extension('hh')
    header = node(path_header)
    Moc(builder.toolkit.qt, header, src)
    obj = Object(src, builder.toolkit, builder.config)
    return obj

Builder.register_deps_handler(deps_handler_name, deps_handler)
Node.extensions['moc.cc'] = Source


class Moc(Builder):

    name = 'MetaObject compilation'

    def __init__(self, qt, src, tgt):

        Builder.__init__(self, [src], [tgt])
        self.qt = qt
        self.src = src
        self.tgt = tgt

    def execute(self):

        return self.cmd(
            'Moc %s' % self.tgt.path(),
            ['%s/bin/moc' % self.qt.prefix, str(self.src.path()),
             '-o', str(self.tgt.path())])


class Ui(Node):

    def __init__(self, path):

        Node.__init__(self, path)

Node.extensions['ui'] = Ui


class Uic(Builder):

    name = 'Qt UI compilation'

    def __init__(self, qt, src, tgt):

        Builder.__init__(self, [src], [tgt])
        self.qt = qt
        self.src = src
        self.tgt = tgt

    def execute(self):

        return self.cmd('Uic %s' % self.tgt, '%s/bin/uic %s -o %s', self.qt.prefix, self.src.path(), self.tgt.path())
