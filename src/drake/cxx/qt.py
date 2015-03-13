# Copyright (C) 2009-2014, Quentin "mefyl" Hocquet
#
# This software is provided "as is" without warranty of any kind,
# either expressed or implied, including but not limited to the
# implied warranties of fitness for a particular purpose.
#
# See the LICENSE file for more information.

import drake
import platform
import re

from itertools import chain

from .. import Builder, Exception, Node, Path, node, debug
from .  import Config, StaticLib, Header, Object, Source

import drake.cxx

deps_handler_name = 'drake.cxx.qt.moc'


def moc_file(linker, header):
  path = Path(header.name()).with_extension('moc.cc')
  src = node(path)
  if src.builder is None:
    Moc(linker.toolkit.qt, header, src)
  return Object(src, linker.toolkit, linker.config)

class Qt:
  def __init__(self,
               toolkit,
               prefix = None,
               gui = False,
               version = drake.Version(4, 8, 6)):
    # XXX: Use QT_VERSION_STR (from Qt/qglobal.h) to determine the version at
    # the given prefix.
    self.plug(toolkit)
    self.files = {}
    self.__moc_cache = {}
    self.__dependencies = {}
    self.__version = version
    if prefix is None:
      test = ['/usr', '/usr/local']
    else:
      test = [prefix]
    beacon = Path('Qt/qconfig.h')
    subdirs = [Path('.'), Path('qt4')]
    if toolkit.os == drake.os.windows:
      lib_suffix = self.__version.major
    else:
      lib_suffix = ''
    for path in test:
      p = Path(path)
      if not p.absolute:
        p = drake.path_source() / p
      for subdir in subdirs:
        if (p / 'include' / subdir / beacon).exists():
          self.prefix = Path(path)
          include_path = self.prefix / 'include' / subdir
          self.__cfg = Config()
          self.__cfg.add_system_include_path(include_path)
          self.__cfg.add_system_include_path(include_path / 'Qt')
          self.__cfg.add_system_include_path(include_path / 'QtCore')
          self.__cfg_gui = Config()
          self.__cfg_gui.add_system_include_path(include_path / 'QtGui')
          self.__cfg_gui.lib('QtGui%s' % lib_suffix)
          self.__cfg_phonon = Config()
          self.__cfg_phonon.lib('phonon%s' % lib_suffix)
          self.__cfg_xml = Config()
          self.__cfg_xml.add_system_include_path(include_path / 'QtXml')
          self.__cfg_xml.lib('QtXml%s' % lib_suffix)
          self.__cfg_network = Config()
          self.__cfg_network.add_system_include_path(include_path / 'QtNetwork')
          self.__cfg_network.lib('QtNetwork%s' % lib_suffix)
          if (include_path / 'QtWebKit').exists():
            self.__cfg_webkit = Config()
            self.__cfg_webkit.add_system_include_path(include_path / 'QtWebKit')
            self.__cfg_webkit.lib('QtWebKit%s' % lib_suffix)
          self.__cfg.lib_path(self.prefix / 'lib' / subdir)
          self.__cfg.lib('QtCore%s' % lib_suffix)

          return
    raise Exception('unable to find %s in %s' % (beacon, ', '.join(test)))

  @property
  def config(self):
    return self.__cfg

  @property
  def config_gui(self):
    return self.__cfg_gui

  @property
  def config_phonon(self):
    return self.__cfg_phonon

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
    # When an object is compiled, search Q_OBJECT in all included
    # headers and remember created moc files.
    tk.hook_object_deps_add(self.hook_object_deps)
    # When a binary is compiled, add all relevant moc files.
    tk.hook_bin_deps_add(self.hook_bin_deps)
    # Handle ui and rc files.
    tk.hook_bin_src_add(self.hook_bin_src)
    tk.qt = self

  moc_re = re.compile(b'Q_OBJECT')

  def hook_bin_src(self, src):
    if isinstance(src, Ui):
      p = Path(src.name())
      p = p.with_extension('%s.hh' % p.extension)
      res = Header(p)
      Uic(self, src, res)
      return res
    elif isinstance(src, Rc):
      p = Path(src.name())
      p = p.with_extension('%s.cc' % p.extension)
      res = Source(p)
      Rcc(self, src, res)
      return res

  def hook_object_deps(self, compiler):
    obj = compiler.object
    if obj in self.__dependencies:
      self.__dependencies[obj] = []
    for source in chain(
        compiler.sources().values(),
        (h for h, local in compiler.header_dependencies if local)):
      if source in self.__moc_cache:
        res = self.__moc_cache[source]
      else:
        res = None
        for line in open(str(source.path()), 'rb'):
          if re.search(self.moc_re, line):
            res = moc_file(compiler, source)
            break
        self.__moc_cache[source] = res
      if res is not None:
        self.__dependencies.setdefault(obj, [])
        self.__dependencies[obj].append(res)
        compiler.add_dynsrc(deps_handler_name, res, source = False)

  def hook_bin_deps(self, compiler):
    for source in list(chain(compiler.sources().values(),
                             compiler.sources_dynamic())):
      for dep in self.__dependencies.get(source, ()):
        compiler.add_dynsrc(deps_handler_name, dep)

def deps_handler(builder, path_obj, t, data):
  if isinstance(builder, (drake.cxx.Linker, drake.cxx.DynLibLinker)):
    if path_obj in drake.Drake.current.nodes:
      return node(path_obj)
    path_src = Path(path_obj).with_extension('moc.cc')
    src = node(path_src)
    path_header = Path(path_obj).with_extension('hh')
    header = node(path_header)
    Moc(builder.toolkit.qt, header, src)
    obj = Object(src, builder.toolkit, builder.config)
    return obj
  elif isinstance(builder, drake.cxx.Compiler):
    source = builder.source
    # FIXME: factor filling of __moc_cache
    res = moc_file(builder, t(path_obj))
    qt = builder.toolkit.qt
    qt._Qt__moc_cache[source] = res
    o = builder.object
    qt._Qt__dependencies.setdefault(o, [])
    qt._Qt__dependencies[o].append(res)
  else:
    raise Exception('unexpected Moc dependency for %s' % builder)

Builder.register_deps_handler(deps_handler_name, deps_handler)
Node.extensions['moc.cc'] = Source

class Moc(Builder):

  def __init__(self, qt, src, tgt):
    Builder.__init__(self, [src], [tgt])
    self.qt = qt
    self.src = src
    self.tgt = tgt

  def execute(self):
    return self.cmd(
      'Moc %s' % self.tgt.path(),
      ['%s/bin/moc' % self.qt.prefix,
       str(self.src.path()),
       '-o', str(self.tgt.path())])

  def __str__(self):
    return 'Moc of %s' % self.src

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
      return self.cmd('Uic %s' % self.tgt,
                      ['%s/bin/uic' % self.qt.prefix,
                       self.src.path(),
                       '-o',
                       self.tgt.path()])

class Rc(Node):

  def __init__(self, path):
    Node.__init__(self, path)

Node.extensions['qrc'] = Rc

class Rcc(Builder):
  name = 'Qt resources compilation'

  def __init__(self, qt, src, tgt):
    Builder.__init__(self, [src], [tgt])
    self.qt = qt
    self.src = src
    self.tgt = tgt

  def execute(self):
    return self.cmd('Rcc %s' % self.tgt,
                    ['%s/bin/rcc' % self.qt.prefix,
                     '-name',
                     'resources',
                     self.src.path(),
                     '-o',
                     self.tgt.path()])
