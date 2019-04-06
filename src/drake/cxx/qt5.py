# Copyright (C) 2009-2016, Quentin "mefyl" Hocquet
#
# This software is provided "as is" without warranty of any kind,
# either expressed or implied, including but not limited to the
# implied warranties of fitness for a particular purpose.
#
# See the LICENSE file for more information.

import itertools
import platform
import re
import os

import drake
import drake.cxx
import drake.sched

from itertools import chain
from drake.utils import pretty_listing

logger = drake.sched.logger

deps_handler_name = 'drake.cxx.qt.moc'

def moc_file(qt, linker, source):
  for i in linker.config.system_include_path:
    if source.name_relative.dirname() == i:
      return None
  path = drake.Path(source.name()).with_extension('moc.cc')
  src = drake.node(path)
  if src.builder is None:
    Moc(linker.toolkit.qt, source, src)
  res = drake.cxx.Object(src, linker.toolkit, linker.config)
  qt._Qt__moc_cache[source] = res
  qt._Qt__moc_files.add(res)
  return res

class Qt(drake.Configuration):
  __libraries = {
    'core': 'Core',
    'declarative': 'Declarative',
    'gui': 'Gui',
    'multimedia': 'Multimedia',
    'network': 'Network',
    'webkit': 'WebKit',
    'widgets': 'Widgets',
    'xml': 'Xml',
    'xml_patterns': 'XmlPatterns',
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
               prefer_shared = True,
               rcc = None,
               qmake = None,
               uic = None,
               moc = None):
    """Find and create a configuration for Qt.

    prefix -- Where to find Qt, should contain
              include/Qt/qglobal.h among others. /usr and
              /usr/local are searched by default. If relative, it
              is rooted in the source tree.
    version -- Requested version.
    prefer_shared -- Check dynamic libraries first.
    """
    self.__rcc = rcc
    self.__qmake = qmake
    self.__uic = uic
    self.__moc = moc
    self.__moc_cache = {}
    self.__dependencies = {}
    self.__moc_files = set()
    cxx_toolkit = cxx_toolkit or drake.cxx.Toolkit()
    self.__cxx_toolkit = cxx_toolkit
    self.__prefer_shared = prefer_shared
    include_subdirs = set(drake.Path(p)
                          for p in ['include', 'include/qt5'])
    if prefix is None:
      test = [path.dirname() for path in cxx_toolkit.include_path
              if path.basename() in include_subdirs]
    else:
      test = [drake.Path(prefix)]
    for i in range(len(test)):
      if not test[i].absolute():
        test[i] = drake.path_source() / test[i]
    token = drake.Path('QtCore/qglobal.h')
    tokens = list(map(lambda p: p / token, include_subdirs))
    prefixes = self._search_many_all(tokens, test)
    miss = []
    # Try every search path
    for path, include_subdir in prefixes:
      include_subdir = include_subdir.without_suffix(token)
      # Create basic configuration for version checking.
      cfg = drake.cxx.Config()
      cfg.pic = True
      if not path.absolute():
        path = path.without_prefix(drake.path_build())
      cfg.add_system_include_path(path / include_subdir)
      if version_effective is None:
        try:
          version_eff = cxx_toolkit.preprocess(
            '#include <QtCore/qglobal.h>\nQT_VERSION',
            config = cfg).split('\n')[-2].strip()
          version_eff = eval(version_eff, {"__builtins__": None}, {})
          version_eff = drake.Version(
            version_eff >> 16,
            (version_eff >> 8) % 256,
            version_eff % 256,
          )
        # If the token doesn't exists, ignore.
        except Exception as e:
          continue
      else:
        version_eff = version_effective
      if version_eff not in version:
        miss.append(version_eff)
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
      self.__version = version_eff
      Qt.__include_dir = path / include_subdir
      return

    raise Exception(
      'no matching Qt ({}) in {}. Found versions: {}.'.format(
        pretty_listing(version, any = True),
        pretty_listing([p for p, _ in prefixes], quantifier = True),
        pretty_listing(miss)))

  @property
  def moc(self):
    if self.__moc is None:
      self.__moc = drake.Node('%s/bin/moc' % self.__prefix)
    return self.__moc

  @property
  def uic(self):
    if self.__uic is None:
      self.__uic = drake.Node('%s/bin/uic' % self.__prefix)
    return self.__uic

  @property
  def rcc(self):
    if self.__rcc is None:
      self.__rcc = drake.Node('%s/bin/rcc' % self.__prefix)
    return self.__rcc

  @property
  def qmake(self):
    if self.__qmake is None:
      self.__qmake = drake.Node('%s/bin/qmake' % self.__prefix)
    return self.__qmake

  def __find_lib(self, lib, cxx_toolkit, static):
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
    tests = []
    for lib, suffix, variant in itertools.product(lib, suffixes, variants):
      libname = '%s%s%s' % (lib, variant, suffix)
      if static:
        filename = cxx_toolkit.libname_static(self.__cfg, libname)
        tests.append(filename)
      else:
        prefix = '' if cxx_toolkit.os == drake.os.windows else 'lib'
        filename = cxx_toolkit.libname_dyn(libname,
                                           self.__cfg, prefix)
        # Test .so.$major first as it is the SONAME.
        tests.append(drake.Path('%s.%s' % (filename,
                                           self.__version.major)))
        tests.append(drake.Path('%s.%s' % (filename, self.__version)))
        tests.append(drake.Path('%s.%s.%s' % (filename,
                                              self.__version.major,
                                              self.__version.minor)))
        tests.append(filename)
    search_path = [self.__prefix / p for p in ['lib/qt5', 'lib']]
    prefix, lib = self._search_many_one(tests, search_path)
    if static:
      return drake.cxx.StaticLib(prefix / lib)
    else:
      return drake.cxx.DynLib(prefix / lib)

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
      p = drake.Path(src.name())
      p = p.with_extension('%s.hh' % p.extension)
      res = drake.cxx.Header(p)
      Uic(self, src, res)
      return res
    elif isinstance(src, Rc):
      p = drake.Path(src.name())
      p = p.with_extension('%s.cc' % p.extension)
      res = drake.cxx.Source(p)
      Rcc(self, src, res)
      return res

  def hook_object_deps(self, compiler):
    obj = compiler.object
    if obj in self.__moc_files:
      return
    with logger.log(
        'drake.cxx.qt',
        drake.log.LogLevel.dump,
        '%s: look for MOC dependencies', compiler):
      self.__dependencies[obj] = []
      for s in chain(
          compiler.sources().values(),
          (h for h, local in compiler.header_dependencies if local)):
        res = self.__moc_cache.get(s)
        if res is None:
          with open(str(s.path()), 'rb') as f:
            for i, line in enumerate(f):
              if re.search(self.moc_re, line):
                logger.log(
                  'drake.cxx.qt',
                  drake.log.LogLevel.trace,
                  '%s: MOC because of line %s: %s', s, i, line.decode().strip())
                res = moc_file(self, compiler, s)
                break
        if res is not None:
          logger.log(
            'drake.cxx.qt',
            drake.log.LogLevel.debug,
            '%s: reference MOC %s', compiler, res)
          self.__dependencies[obj].append(res)
          compiler.add_dynsrc(deps_handler_name, res, source = False)

  def hook_bin_deps(self, compiler):
    def resolve(o):
      return resolve(o) if isinstance(o.builder, drake.Converter) else o
    for o in list(
        resolve(s)
        for s in chain(compiler.sources().values(), compiler.sources_dynamic())
        if isinstance(s, drake.cxx.Object)):
      for moc in self.__dependencies.get(o, ()):
        with logger.log(
            'drake.cxx.qt',
            drake.log.LogLevel.trace,
            '%s: link MOC\'d file %s because of %s', compiler, moc, o):
          compiler.add_dynsrc(deps_handler_name, moc)


for prop, library in Qt._Qt__libraries.items():
  def unclosure(prop, library):
    def library_getter(self, static):
      name = '_Qt__%s_%s' % (prop, 'static' if static else 'dynamic')
      if getattr(self, name) is None:
        lib = self._Qt__find_lib('Qt5{}'.format(library),
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
        c = drake.cxx.Config()
        macro = prop.upper()
        macro += static and '_STATIC' or '_DYN'
        c.define('Qt_%s_LINK' % macro, 1)
        c.add_system_include_path(
          Qt._Qt__include_dir / 'Qt{}'.format(Qt._Qt__libraries[prop]))
        setattr(self, name + '_header', drake.cxx.Config(c))
        c.library_add(lib)
        setattr(self, name, drake.cxx.Config(c))
      return getattr(self, name + ('_header' if not link else ''))
    setattr(Qt, 'config_%s' % prop, config_getter)
  unclosure(prop, library)

def deps_handler(builder, path_obj, t, data):
  if isinstance(builder, (drake.cxx.Linker, drake.cxx.DynLibLinker)):
    with logger.log(
        'drake.cxx.qt',
        drake.log.LogLevel.debug,
        '%s: retrieve linkage to MOC %s', builder, path_obj):
      if path_obj in drake.Drake.current.nodes:
        return drake.node(path_obj)
      path_src = drake.Path(path_obj).with_extension('moc.cc')
      src = drake.node(path_src)
      path_header = drake.Path(path_obj).with_extension('hh')
      header = drake.node(path_header)
      Moc(builder.toolkit.qt, header, src)
      obj = drake.cxx.Object(src, builder.toolkit, builder.config)
      return obj
  elif isinstance(builder, drake.cxx.Compiler):
    with logger.log(
        'drake.cxx.qt',
        drake.log.LogLevel.debug,
        '%s: retrieve reference to MOC %s', builder, path_obj):
      source = drake.node(drake.Path(path_obj).with_extension('moc.hh'))
      qt = builder.toolkit.qt
      res = qt._Qt__moc_cache.get(source)
      if res is None:
        res = moc_file(qt, builder, source)
      if res is not None:
        qt._Qt__dependencies.setdefault(builder.object, []).append(res)
        logger.log(
          'drake.cxx.qt',
          drake.log.LogLevel.debug,
          '%s: reference MOC %s', builder, res)
  else:
    raise Exception('unexpected Moc dependency for %s' % builder)

drake.Builder.register_deps_handler(deps_handler_name, deps_handler)
drake.Node.extensions['moc.cc'] = drake.cxx.Source

class Moc(drake.Builder):

  def __init__(self, qt, src, tgt):
    drake.Builder.__init__(self, [src, qt.moc], [tgt])
    self.__qt = qt
    self.__src = src
    self.__tgt = tgt

  @property
  def command(self):
    return [str(self.__qt.moc),
            str(self.__src.path()),
            '-o', str(self.__tgt.path()),
    ]

  def execute(self):
    return self.cmd(
      'Moc %s' % self.__tgt.path(),
      self.command)

  def __str__(self):
    return 'Moc of %s' % self.__src

  def hash(self):
    """A hash for this builder"""
    return self.command


class Ui(drake.Node):

  def __init__(self, path):
    drake.Node.__init__(self, path)

drake.Node.extensions['ui'] = Ui

class Uic(drake.Builder):
    name = 'Qt UI compilation'

    def __init__(self, qt, src, tgt):
      drake.Builder.__init__(self, [src, qt.uic], [tgt])
      self.__qt = qt
      self.__src = src
      self.__tgt = tgt

    def execute(self):
      return self.cmd('Uic %s' % self.__tgt,
                      [str(self.__qt.uic),
                       self.__src.path(),
                       '-o',
                       self.__tgt.path()])


class Rc(drake.Node):

  def __init__(self, path):
    drake.Node.__init__(self, path)

drake.Node.extensions['qrc'] = Rc

class Rcc(drake.Builder):
  name = 'Qt resources compilation'

  def __init__(self, qt, src, tgt):
    drake.Builder.__init__(self, [src, qt.rcc], [tgt])
    self.__qt = qt
    self.__src = src
    self.__tgt = tgt

  @property
  def command(self):
    return [str(self.__qt.rcc),
            '-name',
            'resources',
            self.__src.path(),
            '-o',
            self.__tgt.path()]

  def execute(self):
    return self.cmd('Rcc %s' % self.__tgt, self.command)

  def hash(self):
    """A hash for this builder"""
    return self.command
