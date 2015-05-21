# Copyright (C) 2009-2014, Quentin "mefyl" Hocquet
#
# This software is provided "as is" without warranty of any kind,
# either expressed or implied, including but not limited to the
# implied warranties of fitness for a particular purpose.
#
# See the LICENSE file for more information.

import itertools
import drake
import platform
import re
import os

from itertools import chain

from .. import Builder, Exception, Node, Path, node, debug
from .  import Config, StaticLib, Header, Object, Source
from .qt_headers import headers as per_version_headers
import drake.cxx

deps_handler_name = 'drake.cxx.qt.moc'

def moc_file(linker, header):
  for i in linker.config.system_include_path:
    if header.name_relative.dirname() == i:
      return None
  path = Path(header.name()).with_extension('moc.cc')
  src = node(path)
  if src.builder is None:
    Moc(linker.toolkit.qt, header, src)
  return Object(src, linker.toolkit, linker.config)

def find(prefix = None,
         cxx_toolkit = None,
         version = drake.Version(),
         prefer_shared = True):
  if isinstance(prefix, Qt):
    if prefix.version not in version:
      raise Exception('given Qt %s does not fit '
                      'the requested version %s' %
                      (prefix.version, version))
    return prefix
  else:
    return Qt(prefix = prefix,
              cxx_toolkit = cxx_toolkit,
              version = version,
              prefer_shared = prefer_shared)

class Qt(drake.Configuration):
  __libraries = {
    'core': 'QtCore',
    'support_3': 'Qt3Support',
    'declarative': 'QtDeclarative',
    'gui': 'QtGui',
    'multimedia': 'QtMultimedia',
    'network': 'QtNetwork',
    'webkit': 'QtWebKit',
    'xml': 'QtXml',
    'xml_pattars': 'QtXmlPatterns',
#    'phonon': 'phonon',
    }

  imageformats = {
    "gif",
    "ico",
    "jpeg",
    "svg",
  }

  def __init__(self,
               cxx_toolkit = None,
               prefix = None,
               version = drake.Version(),
               version_effective = None,
               prefer_shared = True):
    """Find and create a configuration for Qt.

    prefix -- Where to find Qt, should contain
              include/Qt/qglobal.h among others. /usr and
              /usr/local are searched by default. If relative, it
              is rooted in the source tree.
    version -- Requested version.
    prefer_shared -- Check dynamic libraries first.
    """
    self.__moc_cache = {}
    self.__dependencies = {}
    cxx_toolkit = cxx_toolkit or drake.cxx.Toolkit()
    self.__cxx_toolkit = cxx_toolkit
    self.__prefer_shared = prefer_shared
    include_subdirs = set(drake.Path(p)
                          for p in ['include', 'include/qt4'])
    if prefix is None:
      test = [path.dirname() for path in cxx_toolkit.include_path
              if path.basename() in include_subdirs]
    else:
      test = [Path(prefix)]
    for i in range(len(test)):
      if not test[i].absolute():
        test[i] = drake.path_source() / test[i]
    token = drake.Path('Qt/qglobal.h')
    include_subdirs = {drake.Path('include')}
    for prefix in test:
      for subdir in include_subdirs:
        if not (prefix / subdir).exists():
          continue
        include_subdirs = include_subdirs.union(
          (subdir / p for p in (prefix / subdir).list()
           if p.startswith('Qt')))
    tokens = map(lambda p: p / token, include_subdirs)
    prefixes = self._search_many_all(list(tokens), test)
    miss = []
    if version_effective is not None:
      assert prefix is not None
    # Try every search path
    for path, include_subdir in prefixes:
      include_subdir = include_subdir.without_suffix(token)

      # Create basic configuration for version checking.
      cfg = Config()
      cfg.add_system_include_path(path.without_prefix(
        drake.path_build()) / include_subdir)
      self.__lib_path = path / 'lib'
      cfg.lib_path(path.without_prefix(drake.path_build()) / 'lib')
      # XXX: Check the version.
      if version_effective not in version:
        miss.append(version_effective)
        continue
      # Fill configuration
      self.__prefix = path
      self.__cfg = cfg
      for prop in self.__libraries:
        setattr(self, '_Qt__config_%s_dynamic' % prop, None)
        setattr(self, '_Qt__config_%s_static' % prop, None)
        setattr(self, '_Qt__config_%s_dynamic_header' % prop, None)
        setattr(self, '_Qt__config_%s_static_header' % prop, None)
        setattr(self, '_Qt__%s_dynamic' % prop, None)
        setattr(self, '_Qt__%s_static' % prop, None)
      self.__version = version_effective
      self.plug(cxx_toolkit)
      Qt.__include_dir = path.without_prefix(drake.path_build()) / include_subdir
      return

    raise Exception('no matching Qt for the requested version '
                    '(%s) in %s. Found versions: %s.' % \
                    (version, self._format_search([path for path, include_subdir in prefixes]),
                     ', '.join(map(str, miss))))

  def __find_lib(self, lib, lib_path, cxx_toolkit, static):
    suffixes = ['', '-mt']
    # Suffixes
    if static:
      suffixes.append('-mt-s')
      suffixes.append('-s')
    if cxx_toolkit.os == drake.os.windows:
      suffixes = list(map(lambda x: x + str(self.version.major), suffixes))
    if isinstance(cxx_toolkit, drake.cxx.VisualToolkit):
      suffix = '-vc%s0-mt-%s_%s' % (cxx_toolkit.version,
                                    self.version.major,
                                    self.version.minor)
      suffixes = [suffix] + suffixes
      mgw = '-mgw-mt-%s_%s' % (self.version.major, self.version.minor)
      suffixes += [mgw]

    # Variants
    variants = ['']
    if isinstance(lib, str):
      lib = (lib,)
    for lib, suffix, variant in itertools.product(lib, suffixes, variants):
      libname = '%s%s%s' % (lib, variant, suffix)
      tests = []
      if static:
        filename = cxx_toolkit.libname_static(self.__cfg, libname)
        tests.append(lib_path / filename)
      else:
        prefix = '' if cxx_toolkit.os == drake.os.windows else 'lib'
        filename = cxx_toolkit.libname_dyn(libname, self.__cfg, prefix)
        tests.append(lib_path / ('%s.%s' % (filename, self.__version)))
        tests.append(lib_path / filename)
      for test in  tests:
        # Look for a node if we build our own Qt.
        if test.absolute():
          drake_path = test.without_prefix(drake.path_root())
        else:
          drake_path = test
        node = drake.Drake.current.nodes.get(drake_path, None)
        if node is not None:
          return node
        # Otherwise look on the filesystem.
        if test.exists():
          path = os.path.realpath(str(test))
          if static:
            return drake.cxx.StaticLib(path)
          else:
            return drake.cxx.DynLib(path)
    raise Exception(
      'Unable to find %s Qt %s library in %s' % \
      ('static' if static else 'dynamic', lib, lib_path))

  def config(self):
      return self.__cfg

  def __repr__(self):
    return 'Qt(prefix = %s)' % repr(self.__prefix)

  @property
  def version(self):
    return self.__version

  @property
  def prefix(self):
    return self.__prefix

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


for prop, library in Qt._Qt__libraries.items():
  def unclosure(prop, library):
    def library_getter(self, static):
      name = '_Qt__%s_%s' % (prop, 'static' if static else 'dynamic')
      if getattr(self, name) is None:
        lib = self._Qt__find_lib(library,
                                 self._Qt__lib_path,
                                 self._Qt__cxx_toolkit,
                                 static = static)
        setattr(self, name, lib)
      return getattr(self, name)
    setattr(Qt, '%s_dynamic' % prop,
            property(lambda self: library_getter(self, False)))
    setattr(Qt, '%s_static' % prop,
            property(lambda self: library_getter(self, True)))
    def config_getter(self, static = None, link = True):
      if static is None:
        static = not self._Qt__prefer_shared
      name = '_Qt__config_%s_%s' % \
             (prop, 'static' if static else 'dynamic')
      if getattr(self, name) is None:
        lib = library_getter(self, static)
        c = Config()
        macro = prop.upper()
        macro += static and '_STATIC' or '_DYN'
        c.define('Qt_%s_LINK' % macro, 1)
        setattr(self, name + '_header', Config(c))
        c.library_add(lib)
        c.add_system_include_path(Qt._Qt__include_dir /  Qt._Qt__libraries[prop])
        setattr(self, name, Config(c))
      return getattr(self, name + ('_header' if not link else ''))
    setattr(Qt, 'config_%s' % prop, config_getter)
  unclosure(prop, library)

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
    if res is not None:
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

  @property
  def command(self):
    return ['%s/bin/rcc' % self.qt.prefix,
            '-name',
            'resources',
            self.src.path(),
            '-o',
            self.tgt.path()]

  def execute(self):
    return self.cmd('Rcc %s' % self.tgt, self.command)

  def hash(self):
    """A hash for this builder"""
    return self.command
