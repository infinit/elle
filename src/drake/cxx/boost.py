# Copyright (C) 2009-2011, Quentin "mefyl" Hocquet
#
# This software is provided "as is" without warranty of any kind,
# either expressed or implied, including but not limited to the
# implied warranties of fitness for a particular purpose.
#
# See the LICENSE file for more information.

import drake
import drake.cxx
import itertools
import sys

from .. import Exception, Path, Version
from .  import Config, StaticLib


class Boost(drake.Configuration):

  """Configuration for the Boost C++ library collection"""

  __libraries = {
    'test': 'unit_test_framework',
    'thread': 'thread',
    'system': 'system',
    'filesystem': 'filesystem',
    'signals': 'signals',
    'date': 'date_time',
    'regex': 'regex',
    'program': 'program_options',
    'chrono': 'chrono',
    'iostreams': 'iostreams',
    }

  def __init__(self,
               cxx_toolkit = None,
               prefix = None,
               version = Version(),
               prefer_shared = True):
    """Find and create a configuration for Boost.

    prefix -- Where to find Boost, should contain
              include/boost/version.hpp among others. /usr and
              /usr/local are searched by default. If relative, it
              is rooted in the source tree.
    version -- Requested version.
    prefer_shared -- Check dynamic libraries first.
    """
    # Fix arguments
    cxx_toolkit = cxx_toolkit or drake.cxx.Toolkit()
    self.__cxx_toolkit = cxx_toolkit
    self.__prefer_shared = prefer_shared
    # Compute the search path.
    if prefix is None:
      if cxx_toolkit.os == drake.os.windows:
        test = [Path('C:\\Boost')]
      else:
        test = [Path('/usr'), Path('/usr/local')]
    else:
      test = [Path(prefix)]
    for i in range(len(test)):
      if not test[i].absolute():
        test[i] = drake.path_source() / test[i]
    token = drake.Path('boost/version.hpp')
    include_subdirs = {drake.Path('include')}
    for prefix in test:
      for subdir in include_subdirs:
        if not (prefix / subdir).exists():
          continue
        include_subdirs = include_subdirs.union(
          (subdir / p for p in (prefix / subdir).list()
           if p.startswith('boost-')))
    tokens = map(lambda p: p / token, include_subdirs)
    prefixes = self._search_many_all(list(tokens), test)
    miss = []
    # Try every search path
    for path, include_subdir in prefixes:
      include_subdir = include_subdir.without_suffix(token)
      # Create basic configuration for version checking.
      cfg = Config()
      cfg.add_system_include_path(path / include_subdir)
      self.__lib_path = path / 'lib'
      cfg.lib_path(self.__lib_path)
      # Check the version.
      version_eff = cxx_toolkit.preprocess(
          '#include <boost/version.hpp>\nBOOST_VERSION',
          config = cfg)
      version_eff = int(version_eff.split('\n')[-2].strip())
      version_eff = Version(version_eff // 100000,
                            version_eff // 100 % 1000,
                            version_eff % 100)
      if version_eff not in version:
        miss.append(version_eff)
        continue
      # Fill configuration
      self.__prefix = path
      self.__cfg = cfg
      for prop in self.__libraries:
        setattr(self, '_Boost__cfg_%s' % prop, None)
        setattr(self, '_Boost__cfg_%s_static' % prop, None)
      self.__cfg_python = None
      self.__version = version_eff
      return

    raise Exception('no matching boost for the requested version '
                    '(%s) in %s. Found versions: %s.' % \
                    (version, self._format_search(prefixes),
                     ', '.join(map(str, miss))))

  def __find_lib(self, lib, lib_path, cxx_toolkit):
    # Suffixes
    suffixes = ['-mt', '']
    if isinstance(cxx_toolkit, drake.cxx.VisualToolkit):
      suffix = '-vc%s0-mt-%s_%s' % (cxx_toolkit.version,
                                    self.version.major,
                                    self.version.minor)
      suffixes = [suffix] + suffixes
    mgw = '-mgw-mt-%s_%s' % (self.version.major, self.version.minor)
    suffixes += [mgw]
    # Variants
    variants = ['']
    if lib == 'thread' and cxx_toolkit.os is drake.os.windows:
      variants.append('_win32')
    for suffix, variant in itertools.product(suffixes, variants):
      libname = 'boost_%s%s%s' % (lib, variant, suffix)
      filename = cxx_toolkit.libname_dyn(self.__cfg, libname)
      filename_st = cxx_toolkit.libname_static(self.__cfg, libname)
      test_plain = lib_path / filename
      test_plain_st = lib_path / filename_st
      test_versioned = lib_path / ('%s.%s' % (filename, self.__version))
      test_versioned_st = lib_path / ('%s.%s' % (filename_st, self.__version))
      tests_dyn = [
          test_plain,
          test_versioned,

      ]
      tests_st = [
          test_plain_st,
          test_versioned_st,
      ]
      if self.__prefer_shared:
          tests = tests_dyn + tests_st
      else:
          tests = tests_st + tests_dyn
      if cxx_toolkit.os == drake.os.windows:
        test_windows = lib_path / ('lib%s.a' % (libname)) # Force .a on windows
        tests.append(test_windows)
      for test in  tests:
        if test.exists():
          return libname
    raise Exception('Unable to find boost library %s '
                    'in %s' % (lib, lib_path))

  def config(self):
      return self.__cfg

  def config_python(self, static = False):
    if self.__cfg_python is None:
      self.__cfg_python = Config()
      # FIXME: do something smart here
      try:
        self.__cfg_python.lib(self.__find_lib('python-3.2',
                                              self.__lib_path,
                                              self.__cxx_toolkit),
                                              static)
      except:
        self.__cfg_python.lib(self.__find_lib('python3',
                                              self.__lib_path,
                                              self.__cxx_toolkit),
                                              static)
    return self.__cfg_python

  def __repr__(self):
    return 'Boost(prefix = %s)' % repr(self.__prefix)

  @property
  def version(self):
    return self.__version


for prop, library in Boost._Boost__libraries.items():
  def unclosure(prop, library):
    def m(self, static = None):
      if static is None:
          static = not self._Boost__prefer_shared
      pname = '_Boost__cfg_%s%s' % (prop, static and '_static' or '')

      if getattr(self, pname) is None:
        name = self._Boost__find_lib(library,
                                     self._Boost__lib_path,
                                     self._Boost__cxx_toolkit)
        c = Config()
        c.lib(name, static = static)

        c.define('%s_%s_LINK' % (
            library.upper(), static and 'STATIC' or 'DYN'
        ), 1)
        if library == 'unit_test_framework' and not static:
          c.define('BOOST_TEST_DYN_LINK', 1)

        setattr(self, pname, c)
      return getattr(self, pname)
    setattr(Boost, 'config_%s' % prop, m)
  unclosure(prop, library)
