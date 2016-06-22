# Copyright (C) 2009-2016, Quentin "mefyl" Hocquet
#
# This software is provided "as is" without warranty of any kind,
# either expressed or implied, including but not limited to the
# implied warranties of fitness for a particular purpose.
#
# See the LICENSE file for more information.

import collections
import drake
import io
import os
import os.path
import oset
import re
import shutil
import subprocess
import sys
import tempfile

from drake.utils import property_memoize
from itertools import chain

from .. import ShellCommand, Builder, Node, Path, node, cmd, command_add, debug, Expander, FileExpander
from .. import utils
from .. import sched

from drake.sched import logger

def chain(*collections):
  for collection in collections:
    if collection is not None:
      for item in collection:
        yield item

class Config:

    class Library:

        def __init__(self, name, static = False):
            self.name = name
            self.static = static

        def __hash__(self):
            return hash(self.name)

        def __repr__(self):
            return 'Library(%r, %r)' % (self.name, self.static)

    def __init__(self, model = None):
        if model is None:
            self.__debug = False
            self.__export_dynamic = None
            self._includes = {}
            self.__local_includes = sched.OrderedSet()
            self.__optimization = 1
            self.__system_includes = sched.OrderedSet()
            self.__lib_paths = sched.OrderedSet()
            self.__libs = sched.OrderedSet()
            self.__libraries = sched.OrderedSet()
            self.flags = []
            self.ldflags = []
            self._framework = oset.oset()
            self.__defines = collections.OrderedDict()
            self.__standard = None
            self.__rpath = []
            self.__warnings = Config.Warnings()
            self.__use_local_libcxx = None
            self.__whole_archive = False
        else:
            self.__debug = model.__debug
            self.__export_dynamic = model.__export_dynamic
            self._includes = dict(model._includes)
            self.__local_includes = sched.OrderedSet(model.__local_includes)
            self.__optimization = model.__optimization
            self.__system_includes = sched.OrderedSet(model.__system_includes)
            self.__lib_paths = sched.OrderedSet(model.__lib_paths)
            self.__libs = sched.OrderedSet(model.__libs)
            self.__libraries = sched.OrderedSet(model.__libraries)
            self.flags = model.flags[:]
            self.ldflags = model.ldflags[:]
            self._framework = dict(model._framework)
            self.__defines = collections.OrderedDict(model.__defines)
            self.__standard = model.__standard
            self.__rpath = model.__rpath[:]
            self.__warnings = Config.Warnings(model.__warnings)
            self.__use_local_libcxx = model.__use_local_libcxx
            self.__whole_archive = model.__whole_archive
    class Warnings:

      Error = object()

      def __init__(self, model = None):
        if model is None:
          self.__default = True
          self.__warnings = collections.OrderedDict()
        else:
          self.__default = model.__default
          self.__warnings = collections.OrderedDict(model.__warnings)

      def __name(self, name):
        if name not in [
                'address',
                'aggregate_return',
                'array_bounds',
                'cast_align',
                'cast_qual',
                'char_subscripts',
                'clobbered',
                'comment',
                'conversion',
                'coverage_mismatch',
                'cxx11_compat',
                'cxx_compat',
                'delete_non_virtual_dtor',
                'disabled_optimization',
                'double_promotion',
                'empty_body',
                'enum_compare',
                'fatal_errors',
                'float_equal',
                'format',
                'format_nonliteral',
                'format_security',
                'format_y2k',
                'ignored_qualifiers',
                'implicit',
                'implicit_function_declaration',
                'implicit_int',
                'init_self',
                'inline',
                'invalid_pch',
                'jump_misses_init',
                'logical_op',
                'long_long',
                'main',
                'maybe_uninitialized',
                'maybe_uninitialized',
                'mismatched_tags',
                'missing_braces',
                'missing_declarations',
                'missing_field_initializers',
                'missing_format_attribute',
                'missing_include_dirs',
                'no_attributes',
                'no_builtin_macro_redefined',
                'no_cpp',
                'no_deprecated',
                'no_deprecated_declarations',
                'no_div_by_zero',
                'no_endif_labels',
                'no_format_contains_nul',
                'no_format_extra_args',
                'no_free_nonheap_object',
                'no_int_to_pointer_cast',
                'no_invalid_offsetof',
                'no_mudflap',
                'no_multichar',
                'no_overflow',
                'no_pedantic_ms_format',
                'no_pointer_to_int_cast',
                'no_poison_system_directories',
                'no_pragmas',
                'no_unused_result',
                'nonnull',
                'overlength_strings',
                'overloaded_virtual',
                'packed',
                'packed_bitfield_compat',
                'padded',
                'parentheses',
                'pedantic_ms_format',
                'pointer_arith',
                'redundant_decls',
                'return_type',
                'sequence_point',
                'shadow',
                'sign_compare',
                'sign_conversion',
                'stack_protector',
                'strict_aliasing',
                'strict_overflow',
                'switch',
                'switch_default',
                'switch_enum',
                'sync_nand',
                'system_headers',
                'trampolines',
                'trigraphs',
                'type_limits',
                'undef',
                'uninitialized',
                'unknown_pragmas',
                'unsafe_loop_optimizations',
                'unsuffixed_float_constants',
                'unused',
                'unused_but_set_parameter',
                'unused_but_set_variable',
                'unused_function',
                'unused_label',
                'unused_local_typedefs',
                'unused_parameter',
                'unused_value',
                'unused_variable',
                'variadic_macros',
                'vector_operation_performance',
                'vla',
                'volatile_register_var',
                'write_strings',
                'zero_as_null_pointer_constant',
        ]:
          raise Exception('unkown warning: %s' % name)
        return name.replace('_', '-')

      def __setattr__(self, name, value):
        try:
          self.__warnings[self.__name(name)] = value
        except:
          return super().__setattr__(name, value)

      def __getattr__(self, name):
        wname = super().__getattr__('_Config__name')(name)
        try:
          return super().__getattr__('_Warnings__warnings')[wname]
        except KeyError:
          return None
        except:
          return (self, name)

      def __bool__(self):
        return self.__default

      def __iter__(self):
        for warning, enable in self.__warnings.items():
          yield (warning, enable)

    def enable_debug_symbols(self, val = True):
        self.__debug = val

    def enable_optimization(self, val = True):
        if val is True:
            self.__optimization = 1
        elif val is False:
            self.__optimization = 0
        else:
            self.__optimization = val

    def define(self, name, value = None):

        self.__defines[name] = value

    def defines(self):
        return self.__defines

    def flag(self, f):

        self.flags.append(f)

    def ldflag(self, f):
      self.ldflags.append(f)

    def framework_add(self, name):

        self._framework[name] = None

    def frameworks(self):

        return self._framework.keys()

    def add_local_include_path(self, path):
      path = drake.Drake.current.prefix / path
      path = path.canonize()
      self.__local_includes.add(path)
      self._includes[path] = None


    def add_system_include_path(self, path):
      path = Path(path)
      # Never include those.
      # FIXME: Unix only
      if path == Path('/include') or path == Path('/usr/include'):
        return
      if not path.absolute():
        path = drake.path_build() / path
      path = path.canonize()
      self.__system_includes.add(path)
      self._includes[path] = None



    def include_path(self):

        return list(self._includes)

    @property
    def local_include_path(self):

        return list(self.__local_includes)

    @property
    def system_include_path(self):

        return list(self.__system_includes)


    @property
    def library_path(self):

        return iter(self.__lib_paths)


    @property
    def whole_archive(self):

        return self.__whole_archive


    def use_whole_archive(self):

        self.__whole_archive = True


    def lib_path(self, path):

        if path == Path('/lib') or path == Path('/usr/lib'):
            return
        p = Path(path)
        # if not p.absolute():
        #     p = drake.path_source() / drake.Drake.current.prefix / p
        if not p.absolute():
            p = drake.Drake.current.prefix / p
        self.__lib_paths.add(p)


    def lib_path_runtime(self, path):

        self.__rpath.append(drake.Path(path))


    def lib(self, lib, static = False):

      if lib in self.__libs:
        if self.__libs[lib].static != static:
          raise Exception('library %s dynamic versus static '
                          'linkage conflict' % lib)
      else:
        self.__libs.add(Config.Library(lib, static))


    def library_add(self, library):
      if not isinstance(library, drake.BaseNode):
        library = drake.node(drake.Path(library))
      self.__libraries.add(library)

    def __add__(self, rhs):
        """Combine two C++ configurations.

        Defines are merged:

        >>> cfg1 = drake.cxx.Config()
        >>> cfg1.define('A', 0)
        >>> cfg2 = drake.cxx.Config()
        >>> cfg2.define('B', 1)
        >>> sum = cfg1 + cfg2
        >>> sum.defines()['A']
        0
        >>> sum.defines()['B']
        1

        Defining twice to the same value is okay:

        >>> cfg2.define('A', 0)
        >>> sum = cfg1 + cfg2
        >>> sum.defines()['A']
        0

        A different value is not:

        >>> cfg1.define('B', 0)
        >>> cfg1 + cfg2
        Traceback (most recent call last):
            ...
        Exception: redefinition of B from 0 to 1
        """
        def merge_bool(attr):
          mine = getattr(self, attr, None)
          hers = getattr(rhs, attr, None)
          if mine is None:
            return hers
          elif hers is None:
            return mine
          else:
            if mine is hers:
              return hers
            else:
              raise Exception('incompatible C++ configuration '
                              'for attribute %s' % attr)

        res = Config(self)
        res.__export_dynamic = merge_bool('export_dynamic')
        res.__use_local_libcxx = merge_bool('_Config__use_local_libcxx')

        for key, value in rhs.__defines.items():
          if key in res.__defines:
            old = res.__defines[key]
            if old != value:
              raise Exception('redefinition of %s from %s to %s' % \
                              (key, old, value))
          else:
            res.__defines[key] = value
        res.__local_includes |= rhs.__local_includes
        res.__system_includes |= rhs.__system_includes
        res._includes.update(rhs._includes)
        res._framework.update(rhs._framework)
        res.__lib_paths |= rhs.__lib_paths
        res.__libs |= rhs.__libs
        res.__libraries |= rhs.__libraries
        res.flags += rhs.flags
        res.ldflags += rhs.ldflags
        std_s = self.__standard
        std_o = rhs.__standard
        if std_s is not None and std_o is not None:
            if std_s is not std_o:
                raise Exception(
                    'merging C++ configurations with incompatible '
                    'standards: %s and %s' % (std_s, std_o))
        res.__standard = std_s or std_o
        return res

    class Standard:
        def __init__(self, name):
            self.__name = name
        def __str__(self):
            return 'C++ %s' % self.__name

    cxx_14 = Standard('14')
    cxx_11 = Standard('11')
    cxx_0x = Standard('0x')
    cxx_98 = Standard('98')

    @property
    def standard(self):
        return self.__standard

    @standard.setter
    def standard(self, value):
        assert value is None or isinstance(value, Config.Standard)
        self.__standard = value

    @property
    def warnings(self):
        return self.__warnings

    def enable_warnings(self, value):
        self.__warnings.default = value

    @property
    def export_dynamic(self):
        return self.__export_dynamic

    @export_dynamic.setter
    def export_dynamic(self, val):
        self.__export_dynamic = bool(val)

    @property
    def libs(self):
        return list(self.__libs)

    @property
    def libs_dynamic(self):
        return self.__libs_filter(False)

    @property
    def libs_static(self):
        return self.__libs_filter(True)

    def __libs_filter(self, f):
      return list(lib.name for lib in self.__libs if lib.static is f)

    @property
    def libraries(self):
      return tuple(self.__libraries)

    @property
    def use_local_libcxx(self):
        return bool(self.__use_local_libcxx)

    @use_local_libcxx.setter
    def use_local_libcxx(self, val):
        self.__use_local_libcxx = bool(val)

    def __repr__(self):
      content = {}
      if self._includes:
        content['includes'] = self._includes.keys()
      if self.__defines:
        content['defines'] = self.__defines.keys()
      if self.libraries:
        content['libraries'] = self.libraries
      if self.__lib_paths:
        content['libraries path'] = self.__lib_paths
      content_str = map(
        lambda k: '%s = [%s]' % (k, ', '.join(map(repr, content[k]))),
        sorted(content.keys()))
      return 'drake.cxx.Config(%s)' % ', '.join(content_str)

# FIXME: Factor node and builder for executable and staticlib

class _ToolkitType(type):
  def __call__(c, *args, **kwargs):
    if c is Toolkit:
      import platform
      if platform.system() == 'Windows':
        return VisualToolkit()
      else:
        return GccToolkit()
    return type.__call__(c, *args, **kwargs)


class Toolkit(metaclass = _ToolkitType):

    def __init__(self):
        self.includes = []
        self._hook_object_deps = []
        self._hook_bin_deps = []
        self._hook_bin_src = []

    @classmethod
    def default(self):

        return GccToolkit()

    def hook_object_deps_add(self, f):

        self._hook_object_deps.append(f)

    def hook_object_deps(self):

        return self._hook_object_deps

    def hook_bin_deps_add(self, f):

        self._hook_bin_deps.append(f)

    def hook_bin_deps(self):

        return self._hook_bin_deps

    def hook_bin_src_add(self, f):

        self._hook_bin_src.append(f)

    def hook_bin_src(self):

        return self._hook_bin_src

def concatenate(chunks, prefix = ''):
  return ''.join(map(lambda v: ' %s%s' % (prefix,
                                          utils.shell_escape(v)),
                     chunks))

class GccToolkit(Toolkit):

  class Kind(drake.Enumerated,
             values = ['gcc', 'clang']):
    pass

  def __init__(self,
               compiler = None,
               compiler_c = None,
               os = None,
               archiver = None,
               archiver_flags = [],
               ranlib = None):
    Toolkit.__init__(self)
    self.os = os
    self.__include_path = None
    self.__recursive_linkage = False
    self.cxx = compiler or 'g++'
    self.__patchelf = drake.Path('patchelf')
    self.__splitted = None
    try:
      version = subprocess.check_output([self.cxx, '--version'])
    except:
      raise Exception('Unable to find compiler: %s' % self.cxx)
    apple, win32, win64, linux, android, gnuc, clang, x86_64, arm = \
      self.preprocess_isdef((
        '__APPLE__',
        '_WIN32',
        '_WIN64',
        '__linux__',
        '__ANDROID__',
        '__GNUC__',
        '__clang__',
        '__x86_64__',
        '__arm__',
      ))
    if self.os is None:
      if x86_64:
        self.architecture = drake.architecture.x86_64
      elif arm:
        self.architecture = drake.architecture.arm
      else:
        self.architecture = drake.architecture.x86
      if apple:
        mac_os, iphone, iphone_simulator = self.preprocess_istrue(
          ('TARGET_OS_MAC', 'TARGET_OS_IPHONE', 'TARGET_IPHONE_SIMULATOR'),
          preamble = '#include "TargetConditionals.h"')
        if iphone and iphone_simulator:
          self.os = drake.os.ios_simulator
        elif iphone:
          self.os = drake.os.ios
        else:
          self.os = drake.os.macos
      elif win32 or win64:
        self.os = drake.os.windows
        if win64:
          self.architecture = drake.architecture.x86_64
        else:
          self.architecture = drake.architecture.x86
      elif android:
        self.os = drake.os.android
      elif linux:
        self.os = drake.os.linux
      else:
        raise Exception("Host OS not found")
    if clang:
      self.__kind = GccToolkit.Kind.clang
    elif gnuc:
      self.__kind = GccToolkit.Kind.gcc
      vars = ['__GNUC__', '__GNUC_MINOR__', '__GNUC_PATCHLEVEL__']
      self.__version = tuple(map(int, self.preprocess_values(vars)))
    else:
      raise Exception('unknown GCC kind')
    self.c = compiler_c or '%sgcc%s' % (self.prefix, self.suffix)
    if archiver:
      self.ar = archiver
    else:
      self.ar = '%sar' % self.prefix
    if archiver_flags:
      self.ar_flags = archiver_flags
    else:
      self.ar_flags = ['crs']
    if ranlib is None:
      self.ranlib = '%sranlib' % self.prefix
    else:
      self.ranlib = ranlib
    if self.os == drake.os.windows:
      self.res = '%swindres' % self.prefix

  def preprocess_istrue(self, vars, **kwargs):
    return map(lambda e: bool(int(e)),
               self.preprocess_values(vars, **kwargs))

  def preprocess_isdef(self, vars, **kwargs):
    return map(lambda e: e[0] != e[1],
               zip(vars, self.preprocess_values(vars, **kwargs)))

  def preprocess_isdef(self, vars, config = Config(), preamble = None):
    if preamble:
      content = preamble + '\n'
    else:
      content = ''
    content += '\n'.join(reversed(vars))
    content = self.preprocess(content, config).strip().split('\n')
    return map(lambda e: e[0] != e[1], zip(vars, reversed(content)))

  def preprocess_values(self, vars, config = Config(), preamble = None):
    if preamble:
      content = preamble + '\n'
    else:
      content = ''
    content += '\n'.join(reversed(vars))
    content = self.preprocess(content, config).strip().split('\n')
    return reversed(content[-len(vars):])

  def preprocess(self, code, config = Config()):
    cmd = [self.cxx]
    cmd += self.cppflags(config)
    cmd +=['-x',  'c++', '-E', '-']
    p = subprocess.Popen(cmd,
                         stdin = subprocess.PIPE,
                         stdout = subprocess.PIPE,
                         stderr = subprocess.PIPE)
    stdout, stderr = p.communicate(code.encode('utf-8'))
    if p.returncode != 0:
        raise Exception(
            'Preprocessing failed with return code %s.'
            '\nInput:\n%s\nStderr:\n%s\n' % \
            (p.returncode, code, stderr))
    return stdout.decode("utf-8")

  def enable_recursive_linkage(self, doit = True):
      self.__recursive_linkage = doit

  def object_extension(self):

      return 'o'

  def cppflags(self, cfg):
      res = []
      for name, v in cfg.defines().items():
          if v is None:
              res.append('-D%s' % name)
          else:
              res.append('-D%s=%s' % (name, utils.shell_escape(v)))
      for include in cfg.system_include_path:
          res.append('-isystem')
          res.append(utils.shell_escape(include))
          res.append('-isystem')
          res.append(utils.shell_escape(drake.path_source() / include))
      for include in cfg.local_include_path:
          res.append('-I')
          res.append(utils.shell_escape(drake.path_source() / include))
          res.append('-I')
          res.append(utils.shell_escape(include))
      return res

  def cflags(self, cfg):
      res = []
      if cfg._Config__optimization:
        res.append('-O2')
      if cfg._Config__debug:
        res.append('-g')
      std = cfg._Config__standard
      if std is None:
        pass
      elif std is Config.cxx_98:
        res.append('-std=c++98')
      elif std is Config.cxx_0x:
        res.append('-std=c++0x')
      elif std is Config.cxx_11:
        res.append('-std=c++11')
      elif std is Config.cxx_14:
        if self.__kind is GccToolkit.Kind.gcc \
           and self.__version[:2] == (4, 8):
          # GCC 4.8 does not recognize std=c++14, but enables the
          # features nonetheless with std=c++11
          res.append('-std=c++11')
        else:
          res.append('-std=c++14')
      else:
          raise Exception('Unknown C++ standard: %s' % std)
      if cfg.warnings:
          res.append('-Wall')
      for warning, enable in cfg.warnings:
        if self.__kind is GccToolkit.Kind.gcc and warning in [
            'mismatched-tags',
        ]:
            continue
        if enable is None:
          pass
        elif enable is Config.Warnings.Error:
          prefix = 'error='
        elif not enable:
          prefix = 'no-'
        else:
          prefix = ''
        res.append('-W%s%s' % (prefix, warning))
      return res

  def ldflags(self, cfg):
    res = []
    if cfg.export_dynamic and self.os not in (drake.os.macos, drake.os.windows, drake.os.ios, drake.os.ios_simulator):
      res.append('-rdynamic')
    if self.os is drake.os.windows:
      # Assume this is mingw. Assume nobody wants to ship those DLL
      # manually. Right ?
      res.append('-static-libgcc')
      res.append('-static-libstdc++')
      res.append('-static')
      res.append('-lwinpthread')
      res.append('-dynamic')
    elif self.os in (drake.os.ios, drake.os.ios_simulator):
      res.append('-static')
    elif self.os is drake.os.linux:
      res.append('-pthread')
    res += cfg.ldflags
    return res

  def compile(self, cfg, src, obj, c = False, pic = False):
    extraflags = []
    if pic and self.os is not drake.os.windows:
      extraflags.append('-fPIC')
    return [c and self.c or self.cxx] + cfg.flags + \
        self.cppflags(cfg) + self.cflags(cfg) + \
        extraflags + ['-c', str(src), '-o', str(obj)]

  def render_resource(self, src, obj):
    return [self.res, src, '-O', 'coff', '-o', str(obj)]

  def archive(self, objs, lib):
    objects = [str(n.path()) for n in objs
               if isinstance(n, drake.cxx.Object)]
    archive_cmd = [self.ar] + self.ar_flags + [str(lib.path())] + objects
    if self.ranlib:
      return (archive_cmd, [self.ranlib] + [str(lib.path())])
    else:
      return archive_cmd

  def __libraries_flags(self, cfg, libraries, cmd):
    for lib in libraries:
      if isinstance(lib, (StaticLib, DynLib)):
        cmd.append(lib.path())
      else:
        raise Exception('cannot link a %s' % type(lib))
      if isinstance(lib, StaticLib):
        cmd += [str(d.path()) for d in lib.dependencies_recursive
                if isinstance(d, (StaticLib, DynLib))]
    if self.__recursive_linkage:
      cmd.append('-Wl,-(')
    for lib in cfg.libs_dynamic:
      cmd.append('-l%s' % lib)
    # XXX Should refer to libraries with path on MacOS.
    if cfg.libs_static:
      if self.os not in (drake.os.macos, drake.os.windows, drake.os.ios, drake.ios_simulator):
        cmd.append('-Wl,-Bstatic')
        for lib in cfg.libs_static:
          cmd.append('-l%s' % lib)
        cmd.append('-Wl,-Bdynamic')
      else:
        # This is some kind of joke from Apple, -Bstatic is a
        # no-op on Lion.
        for lib in cfg.libs_static:
          found = False
          paths = list(cfg.library_path)
          for path in paths:
            for name in ('lib%s.a' % lib, '%s.a' % lib, '%s.lib' % lib, 'lib%s.lib' % lib):
              libpath = path / name
              if libpath.exists():
                cmd.append(libpath)
                found = True
                break
          if not found:
            raise Exception('can\'t find static version of %s' % lib)
    if self.__recursive_linkage:
      cmd.append('-Wl,-)')

  def __compute_rpaths(self, exe, cfg, objs):
    rpath = sched.OrderedSet(cfg._Config__rpath)
    rpath_link = sched.OrderedSet()
    prefix = exe.path().dirname()
    if self.os == drake.os.linux:
      for lib in (lib for lib in objs if isinstance(lib, DynLib)):
        for library in lib.dynamic_libraries:
          rpath.add(library.path().dirname().without_prefix(prefix))
          rpath_link.add(str(library.path().dirname()))
    return rpath, rpath_link

  def link(self, cfg, objs, exe):
      cmd = [self.cxx] + cfg.flags + self.ldflags(cfg)
      for framework in cfg.frameworks():
          cmd += ['-framework', framework]
      for path in cfg.library_path:
          cmd += ['-L', path]
      rpath, rpath_link = self.__compute_rpaths(exe, cfg, objs)
      for path in rpath:
        cmd.append('-Wl,-rpath,%s' % self.rpath(path))
      for path in rpath_link:
        cmd.append('-Wl,-rpath-link,%s' % path)
      if self.os == drake.os.macos:
          cmd += ['-undefined', 'dynamic_lookup']
      for obj in (obj for obj in objs if not isinstance(obj, Library)):
        cmd.append(obj.path())
      cmd += ['-o', exe.path()]
      libraries = (obj for obj in objs if isinstance(obj, Library))
      self.__libraries_flags(cfg, libraries, cmd)
      if cfg.whole_archive:
        pre=list()
        ar=list()
        post=list()
        arnames=list()
        for e in cmd:
          e = str(e)
          if e[-2:] == '.a':
            if e.split('/')[-1] not in arnames:
              ar.append(e)
              arnames.append(e.split('/')[-1])
            else:
              pass
          else:
            if len(ar):
              post.append(e)
            else:
              pre.append(e)
        cmd = pre + ['-Wl,--whole-archive'] + ar + ['-Wl,--no-whole-archive'] + post

      return cmd

  def dynlink(self, cfg, objs, exe):
      cmd = [self.cxx] + cfg.flags + self.ldflags(cfg)
      for framework in cfg.frameworks():
          cmd += ['-framework', framework]
      for path in cfg.library_path:
          cmd += ['-L', path]
      rpath, rpath_link = self.__compute_rpaths(exe, cfg, objs)
      for path in rpath:
        cmd.append('-Wl,-rpath,%s' % self.rpath(path))
      if self.os == drake.os.macos:
        cmd += ['-undefined', 'dynamic_lookup',
                '-Wl,-install_name,@rpath/%s' % exe.path().basename(),
                '-Wl,-headerpad_max_install_names']
      if self.os is drake.os.linux:
        cmd.append('-Wl,-soname,%s' % exe.name().basename())
      to_link = []
      for obj in objs:
          if isinstance(obj, (StaticLib, DynLib)):
            to_link.append(obj)
          else:
            cmd.append(obj.path())
      cmd += ['-shared', '-o', exe.path()]
      self.__libraries_flags(cfg, to_link, cmd)
      if cfg.whole_archive:
        pre=list()
        ar=list()
        post=list()
        arnames=list()
        for e in cmd:
          e = str(e)
          if e[-2:] == '.a':
            if e.split('/')[-1] not in arnames:
              ar.append(e)
              arnames.append(e.split('/')[-1])
            else:
              pass
          else:
            if len(ar):
              post.append(e)
            else:
              pre.append(e)
        cmd = pre
        if self.os is drake.os.windows:
          cmd.append('-Wl,--export-all-symbols')
        cmd += ['-Wl,--whole-archive'] + ar + ['-Wl,--no-whole-archive'] + post
      return cmd

  def libname_static(self, cfg, path):

      path = Path(path)
      return path.dirname() / ('lib%s.a' % str(path.basename()))

  def libname_dyn(self, path, cfg = None, prefix = 'lib'):

      path = Path(path)
      if self.os in [drake.os.linux, drake.os.android]:
        ext = 'so'
      elif self.os in (drake.os.macos, drake.os.ios, drake.os.ios_simulator):
        ext = 'dylib'
      elif self.os == drake.os.windows:
        ext = 'dll'
      else:
        assert False
      return path.dirname() / ('%s%s.%s' % (prefix, path.basename(), ext))

  def libname_module(self, cfg, path):
    if self.os == drake.os.windows:
      ext = 'dll'
    else:
      ext = 'so'
    return Path(path).with_extension(ext)

  def exename(self, cfg, path):
      path = Path(path)
      if self.os == drake.os.windows:
        path = path.with_extension('exe')
      return path

  def rpath_set_command(self, binary, path):
    path = self.rpath(path)
    if self.os in [drake.os.macos, drake.os.ios]:
      return ['install_name_tool',
              '-add_rpath', str(path),
              str(binary)]
    else:
      if isinstance(self.__patchelf, drake.Node):
        patchelf = self.__patchelf.path()
      else:
        patchelf = self.__patchelf
      return [str(patchelf), '--set-rpath', str(path), str(binary)]

  @property
  def patchelf(self):
    return self.__patchelf

  @patchelf.setter
  def patchelf(self, value):
    self.__patchelf = value

  def rpath(self, path):
    path = drake.Path(path)
    if not path.absolute():
      if self.os is drake.os.macos:
        return drake.Path('@loader_path') / path
      else:
        return drake.Path('$ORIGIN') / path
    else:
      return path

  def __split(self):
    if self.__splitted is None:
      if self.__kind is GccToolkit.Kind.gcc:
        name = 'g\+\+'
      elif self.__kind is GccToolkit.Kind.clang:
        name = 'clang\+\+'
      else:
        raise Exception('unknown compiler kind: %s' % self.__kind)
      r = re.compile(
        '(.*)(%s)(-[0-9]+(\.[0-9]+(\.[0-9]+)?)?)?$' % name)
      match = r.match(self.cxx)
      if not match:
        raise Exception('unrecognized compiler name: %s' % self.cxx)
      self.__splitted = [s or '' for s in match.groups()[:3]]
    return self.__splitted

  @property
  def prefix(self):
    return self.__split()[0]

  @property
  def suffix(self):
    return self.__split()[2]

  @property
  def kind(self):
    return self.__kind

  @property
  def include_path(self):
    if self.__include_path is None:
      cmd = [self.cxx, '-v', '-x', 'c++', '-E', '-']
      p = subprocess.Popen(cmd,
                           stdin = subprocess.PIPE,
                           stdout = subprocess.PIPE,
                           stderr = subprocess.PIPE)
      stdout, stderr = p.communicate()
      if p.returncode != 0:
        raise Exception(
              'Could not determine C++ toolkit include path.'
              '\nStderr:\n%s\n' % stderr)
      store = False
      self.__include_path = []
      for line in stderr.split(b'\n'):
        if store:
          if line == b'End of search list.':
            break
          self.__include_path.append(
            drake.Path(line[1:].decode('latin-1')))
        elif line == b'#include <...> search starts here:':
          store = True
    return self.__include_path


class VisualToolkit(Toolkit):

  os = drake.os.windows

  def __init__(self,
               version = 11,
               override_path = None,
               override_include = None):
    Toolkit.__init__(self)
    self.__version = version
    def output(cmd):
      p = subprocess.Popen(cmd, shell = True,
                           stdout = subprocess.PIPE)
      return p.stdout.read().strip().decode('utf-8')
    # Des barres de rire cet OS j'vous dit.
    var = '%%VS%s0COMNTOOLS%%' % version
    cfg = output('echo %s' % var)
    if cfg == var:
        raise Exception('%s is not defined, check your Visual Studio '
                        'installation.' % var)
    cfg = Path(cfg)
    if not cfg.exists():
      raise Exception('Visual Studio configuration file '
                      'does not exist: %s' % cfg)
    cfg = cfg / 'vsvars32.bat'
    tmp = Path(tempfile.mkdtemp())
    cp = tmp / 'fuck.bat'
    shutil.copy(str(cfg), str(cp))
    f = open(str(cp), 'a')
    print('@echo %PATH%', file = f) # Des putain de barres.
    print('@echo %LIB%', file = f)
    print('@echo %INCLUDE%', file = f)
    f.close()
    res = output('"%s"' % cp).split('\r\n')
    path = res[-3].split(';')
    lib = res[-2]
    include = [res[-1]]
    if override_path is not None:
      path = [override_path] + path
    os.environ['PATH'] = ';'.join(path)
    os.environ['LIB'] = lib
    if override_include is not None:
      include = [override_include] + include
    os.environ['INCLUDE'] = ';'.join(include)
    shutil.rmtree(str(tmp))
    self.flags = []

  def preprocess(self, code, config = Config()):

    fd, path = tempfile.mkstemp()
    try:
      with os.fdopen(fd, 'w') as f:
        print(code, file = f)
      cmd = ['cl.exe', '/E', path] + self.cppflags(config)
      p = subprocess.Popen(cmd,
                           stdin = subprocess.PIPE,
                           stdout = subprocess.PIPE,
                           stderr = subprocess.PIPE)
      stdout, stderr = p.communicate()
      if p.returncode != 0:
        raise Exception(
          'Preprocessing failed with return code %s.\n'
          'Input:\n%s\nStderr:\n%s\n' % \
          (p.returncode, code, stderr))
      return stdout.decode("utf-8")
    finally:
      os.remove(path)

  def warning_disable(self, n):
      self.flags.append('/wd%s' % n)

  def object_extension(self):
      return 'obj'

  def cppflags(self, cfg):
    flags = []
    for name, v in cfg.defines().items():
      if v is None:
        flags.append('/D%s' % name)
      else:
        flags.append('/D%s=%s' % (name, utils.shell_escape(v)))
    for path in cfg.system_include_path:
      flags.append('/I%s' % utils.shell_escape(path))
    for path in cfg.local_include_path:
      flags.append('/I%s' % utils.shell_escape(drake.path_source() / path))
      flags.append('/I%s' % utils.shell_escape(path))
    return flags

  def compile(self, cfg, src, obj, c = False, pic = False):
      cmd = ['cl.exe', '/MT', '/TP', '/nologo', '/DWIN32']
      cmd += self.flags
      cmd += cfg.flags
      cmd.append('/EHsc')
      cmd += self.cppflags(cfg)
      cmd.append('/Fo%s' % obj)
      cmd.append('/c')
      cmd.append(str(src))
      return cmd

  def archive(self, cfg, objs, lib):
      return 'lib /nologo /MT %s /OUT:%s' % \
          (' '.join(map(str, objs)), lib)

  def link(self, cfg, objs, exe):
    # /ENTRY:main
    # /SUBSYSTEM:CONSOLE
    return 'cl.exe /nologo /MT %s %s /link /OUT:%s /SUBSYSTEM:CONSOLE %s %s opengl32.lib' % \
           (' '.join(cfg.flags),
            ' '.join(map(str, objs)),
            exe,
            ''.join(map(lambda i : ' /LIBPATH:"%s"' %i, cfg.__lib_paths)),
            ''.join(map(lambda d: ' %s.lib' % d, cfg.__libs)))

  def dynlink(self, cfg, objs, exe):
    return 'link.exe /nologo %s /OUT:%s /dll %s %s' % \
           (' '.join(map(str, objs)),
            exe,
            ''.join(map(lambda i : ' /LIBPATH:"%s"' %i, cfg.__lib_paths)),
            ''.join(map(lambda d: ' %s.lib' % d, cfg.__libs)))

  def libname_static(self, cfg, path):
      path = Path(path)
      return path.dirname() / ('%s.lib' % str(path.basename()))

  def libname_dyn(self, path, cfg = None):
      path = Path(path)
      return path.dirname() / ('%s.dll' % str(path.basename()))

  def libname_module(self, cfg, path):
      path = Path(path)
      return path.dirname() / ('%s.dll' % str(path.basename()))

  def exename(self, cfg, path):
      res = Path(path)
      res.extension = 'exe'
      return res

  @property
  def version(self):
    return self.__version

def deps_handler(builder, path, t, data):
    return node(path, t)

profile_deps = drake.Profile('C++ dependencies exploration')

def inclusion_dependencies(n, toolkit, config):
  with profile_deps():
    search_path = []
    for path, local in chain(
        ((path, True) for path in config.local_include_path),
        ((path, False) for path in config.system_include_path)):
      search_path.append((path, True, local))
    search_path += [(path, False, False)
                    for path in toolkit.include_path]
    cycles_map = dict()
    owner_map = dict()
    cycles, deps = mkdeps(n, tuple(search_path),
                          set(), cycles_map, owner_map)
    deps.add((n, True))
    assert not cycles
    assert not cycles_map
    assert not owner_map
    return deps

__dependencies_includes = {}
__dependencies_result = {}
__include_re = re.compile(b'\\s*#\\s*include\\s*(<|")(.*)(>|")')

def mkdeps(explored_node, search, marks, cycles_map, owner_map,
           user = True):
  # Fetch cached result
  key = (search, explored_node)
  res = __dependencies_result.get(key, None)
  # Check if this node was explored
  path = explored_node.path()
  # If we've been explored ...
  if path in marks:
    # ... if we have no result yet, this is a cycle.
    if res is None:
      cycle = explored_node
      while True:
        logger.log('drake.cxx.dependencies',
                   drake.log.LogLevel.trace,
                   'detected cycle on %s', cycle)
        new_owner = owner_map.get(cycle, None)
        if new_owner is None or new_owner is cycle:
          break
        cycle = new_owner
      cycles_map.setdefault(cycle, set())
      return (set((cycle,)), set())
  else:
    marks.add(path)
  if res is None:
    cycles, deps = _mkdeps(explored_node, search,
                           marks, cycles_map, owner_map, user = user)
    if cycles:
      try:
        cycles.remove(explored_node)
      except KeyError:
        pass
      else:
        if cycles:
          new_cycle = next(iter(cycles))
          logger.log('drake.cxx.dependencies',
                     drake.log.LogLevel.trace,
                     'merge cycle on %s to %s',
                     explored_node, new_cycle)
          cycles_map[new_cycle].update(cycles_map[explored_node])
          for node in cycles_map[explored_node]:
            owner_map[node] = new_cycle
        else:
          logger.log('drake.cxx.dependencies',
                     drake.log.LogLevel.trace,
                     'finalize cycle %s', explored_node)
          for dependency in cycles_map[explored_node]:
            __dependencies_result[(search, dependency)] = deps
            del owner_map[dependency]
        del cycles_map[explored_node]
    else:
      __dependencies_result[key] = deps
    return (cycles, deps)
  else:
    return (set(), res)

def _mkdeps(explored_node, search, marks, cycles_map, owner_map, user = True):
  path = explored_node.path()
  with logger.log('drake.cxx.dependencies',
                  drake.log.LogLevel.trace,
                  'explore dependencies of %s', path):
    cycles = set()
    deps = set()
    matches = __dependencies_includes.get(path, None)
    if matches is None:
      matches = []
      with open(str(path), 'rb') as include_file:
        for line in include_file:
          line = line.strip()
          match = __include_re.match(line)
          if match:
            include = match.group(2).decode('latin-1')
            local = match.group(1) == b'"'
            matches.append((include, local))
      __dependencies_includes[path] = matches
    for include, local in matches:
      if local:
        current_path = explored_node.name_absolute().dirname()
        local_path = ((current_path, True, user),)
      else:
        local_path = ()
      found = None
      for include_path, test_node, user in chain(local_path, search):
        name = include_path / include
        test = name
        if test_node:
          registered = drake.Drake.current.nodes.get(test, None)
          if registered is not None:
            # Check this is not an old cached dependency from
            # cxx.inclusions. Not sure of myself though.
            # if test.is_file() or registered.builder is not None:
            found = drake.node(test)
            logger.log('drake.cxx.dependencies',
                       drake.log.LogLevel.debug,
                       'found %s in the nodes', found)
            break
        # Check if such a file doesn't exist, unregistered, in the
        # source path.
        if not found:# or drake.path_source() != Path('.'):
          test = drake.path_source() / test
          if test.is_file():
            logger.log('drake.cxx.dependencies',
                       drake.log.LogLevel.debug,
                       'found %s in sources', name)
            found = drake.node(name, Header)
            break
      if found is not None:
        try:
          # FIXME: is building a node during dependencies ok ?
          found.build()
        except drake.NoBuilder:
          # If a node is found but cannot be built, it must be an obsolete
          # file pulled from the on-disk dependencies. Discard it and let
          # the compiler err because it can't find the include.
          continue
        deps.add((found, user))
        subcycles, subdeps = mkdeps(found, search,
                                    marks, cycles_map, owner_map,
                                    user = user)
        if not cycles and subcycles:
          my_cycle = next(iter(subcycles))
          cycles_map.setdefault(my_cycle, set()).add(explored_node)
          owner_map[explored_node] = my_cycle
        deps.update(subdeps)
        cycles = cycles.union(subcycles)
      else:
        logger.log('drake.cxx.dependencies',
                   drake.log.LogLevel.trace,
                   'file not found: %s', include)
    return (cycles, deps)

class _Compiler(Builder):

  def __init__(self, src, obj, tk, cfg):

    self.src = src
    self.obj = obj
    self.config = cfg
    self.toolkit = tk
    self.__header_dependencies = set()
    Builder.__init__(self, [src], [obj])

  def dependencies(self):
    for dep, local in inclusion_dependencies(
        self.src, self.toolkit, self.config):
      if dep != self.src:
        self.add_dynsrc(self.deps, dep)
        self.header_dependencies.add((dep, local))
    for hook in self.toolkit.hook_object_deps():
      hook(self)

  def execute(self):
    # FIXME: handle this in the toolkit itself.
    leave_stdout = isinstance(self.toolkit, VisualToolkit)
    return self.cmd('Compile %s' % self.obj, self.command,
                    leave_stdout = leave_stdout)

  def mkdeps(self):
    return {self.src: inclusion_dependencies(self.src, self.config)}

  @property
  def header_dependencies(self):
    return self.__header_dependencies

  @property
  def object(self):
    return self.obj

  @property
  def source(self):
    return self.src

  def __str__(self):
    return 'compilation of %s' % self.obj

  def __repr__(self):
    return 'Compiler(%s)' % self.obj

class ResourceCompiler(_Compiler):

  name = 'C++ resource compilation'
  deps = 'drake.cxx.inclusions'

  Builder.register_deps_handler(deps, deps_handler)

  def __init__(self, src, obj, tk, cfg, c = False):
    super().__init__(src, obj, tk, cfg)

  @property
  def command(self):
    return self.toolkit.render_resource(self.src.path(),
                                        self.obj.path())

class Compiler(_Compiler):

  name = 'C++ compilation'
  deps = 'drake.cxx.inclusions'

  Builder.register_deps_handler(deps, deps_handler)

  def __init__(self, src, obj, tk, cfg, c = False):
    super().__init__(src, obj, tk, cfg)
    self.__c = c

  @property
  def command(self):
    return self.toolkit.compile(self.config,
                                self.src.path(),
                                self.obj.path(),
                                c = self.__c,
                                pic = self.pic)

  @property
  def pic(self):
    def pic_rec(node):
      for consumer in node.consumers:
        if sys.platform == 'win32' or sys.platform == 'gygwin':
          return False
        if isinstance(consumer, DynLibLinker):
          return True
        elif isinstance(consumer, StaticLibLinker):
          if pic_rec(consumer.library):
            return True
      return False
    return pic_rec(self.obj)

  def hash(self):
    return self.command

class Linker(Builder):

  name = 'executable linkage'

  def hash(self):
    return self.command

  def dependencies(self):
    for hook in self.toolkit.hook_bin_deps():
      hook(self)

  def __init__(self, exe, tk, cfg, strip = False):
    # FIXME: factor with DynLibLinker
    self.exe = exe
    self.toolkit = tk
    self.config = drake.cxx.Config(cfg)
    Builder.__init__(self, chain(exe.sources, exe.dynamic_libraries),
                     [exe])
    self.__strip = strip

  def execute(self):
    return self.cmd('Link %s' % self.exe, self.command)

  @property
  def command(self):
    objects = self.exe.sources + list(self.exe.dynamic_libraries)
    cmd = self.toolkit.link(
      self.config,
      objects + list(sorted(self.sources_dynamic())),
      self.exe)
    cmd = (cmd,)
    if self.__strip:
      cmd = cmd + (['%sstrip' % self.toolkit.prefix, self.exe.path()],)
    if self.config.use_local_libcxx and self.toolkit.os == drake.os.macos:
      cmd = cmd + (['install_name_tool',
                   '-change', '/usr/lib/libc++.1.dylib',
                   '@rpath/libc++.1.dylib',
                   self.exe.path()
                   ],)
    return cmd

  def __repr__(self):
    return 'Linker(%s)' % self.exe

  def __str__(self):
    return 'Linker(%s)' % self.exe


class DynLibLinker(Builder):

  name = 'dynamic library linkage'

  def dependencies(self):
    for hook in self.toolkit.hook_bin_deps():
      hook(self)

  def __init__(self, lib, tk, cfg, strip = False):
    self.lib = lib
    self.toolkit = tk
    self.config = Config(cfg)
    self.__library = lib
    # This duplicates self.__sources, but preserves the order.
    self.__objects = lib.sources
    self.__dynamic_libraries = lib.dynamic_libraries
    Builder.__init__(self,
                     chain(self.__objects, lib.dynamic_libraries),
                     [lib])
    self.__strip = strip

  def execute(self):
    return self.cmd('Link %s' % self.lib, self.command)

  @property
  def command(self):
    objects = self.__objects + list(self.__dynamic_libraries)
    cmd = self.toolkit.dynlink(
        self.config,
        objects + list(self.sources_dynamic()),
        self.lib)
    cmd = (cmd,)
    if self.__strip:
      cmd = cmd + ( ['%sstrip' % self.toolkit.prefix, self.lib.path()],)
    if self.config.use_local_libcxx and self.toolkit.os == drake.os.macos:
      cmd = cmd + (['install_name_tool',
                   '-change', '/usr/lib/libc++.1.dylib',
                   '@rpath/libc++.1.dylib',
                   self.lib.path()
                   ],)
    return cmd

  def __repr__(self):
    return 'Linker for %s' % self.lib

  def hash(self):
    return self.command

  def __str__(self):
    return 'dynamic linkage of %s' % self.lib


class StaticLibLinker(ShellCommand):

    name = 'static library archiving'

    def dependencies(self):
      for hook in self.toolkit.hook_bin_deps():
        hook(self)

    def __init__(self, objs, lib, tk, cfg):
      self.objs = objs
      self.__library = lib
      for o in self.objs:
        if isinstance(o, StaticLib):
          lib.dependency_add(o)
      self.toolkit = tk
      self.config = cfg
      super().__init__(objs, [lib], None)

    def execute(self):
      # Our 'ar' command apends files to the target, so we must rm it first
      try:
        os.remove(str(self.__library.path()))
      except:
        pass
      return self.cmd('Archive %s' % self.__library, self.command)

    @property_memoize
    def command(self):
      return self.toolkit.archive(
        self.objs + list(self.sources_dynamic()), self.__library)

    @property
    def library(self):
      return self.__library

    def hash(self):
      return self.command


class Source(Node):

    def __init__(self, path):

        Node.__init__(self, path)

    def clone(self, path):

        return Source(path)

Node.extensions['c'] = Source
Node.extensions['cc'] = Source
Node.extensions['cpp'] = Source
Node.extensions['mm'] = Source
Node.extensions['S'] = Source

class Header(Node):

    def __init__(self, path):

        Node.__init__(self, path)

Node.extensions['h'] = Header
Node.extensions['hh'] = Header
Node.extensions['hpp'] = Header
Node.extensions['hxx'] = Header

class ResourceFile(Node):

    def __init__(self, path):

        Node.__init__(self, path)

    def clone(self, path):

        return ResourceFile(path)

Node.extensions['rc'] = ResourceFile

class Object(Node):

    def __init__(self, *args, **kwargs):

        if len(args) + len(kwargs) == 1:
            self.__init_node__(*args, **kwargs)
        else:
            self.__init_object__(*args, **kwargs)

    def __init_node__(self, path):

        Node.__init__(self, path)

    def __init_object__(self, source, tk, cfg):

        self.source = source
        self.toolkit = tk
        self.cfg = cfg
        path = source.name_relative
        c = path.extension == 'c'
        path = path.without_last_extension()
        if len(path.extension):
          path = path.with_extension('%s.%s' % (path.extension, tk.object_extension()))
        else:
          path = path.with_extension(tk.object_extension())
        Node.__init__(self, path)

        Compiler(source, self, tk, cfg, c = c)

    def mkdeps(self):

        return self._builder.mkdeps()

Node.extensions['o'] = Object

class Resource(Node):

  def __init__(self, source, tk, cfg):

    self.source = source
    self.toolkit = tk

    path = source.name_relative
    path = path.without_last_extension()
    if len(path.extension):
      path = path.with_extension('%s.%s' % (path.extension, 'res'))
    else:
      path = path.with_extension('res')

    Node.__init__(self, path)

    ResourceCompiler(source, self, tk, cfg)

Node.extensions['res'] = Resource

class Binary(Node):

  def __init__(self, path, sources, tk, cfg):
    self.tk = tk
    self.cfg = cfg
    Node.__init__(self, path)
    self.__dynamic_libraries = sched.OrderedSet()
    self.sources = None
    if sources is not None:
      self.sources = []
      for source in sources:
        self.src_add(source, self.tk, self.cfg)
      for lib in cfg.libraries:
        self.src_add(lib, self.tk, self.cfg)

  def clone(self, path):
    return self.__class__(path)

  @property
  def dynamic_libraries(self):
    return self.__dynamic_libraries

  def src_add(self, source, tk, cfg):
    if source.__class__ == Object:
      self.sources.append(source)
    elif source.__class__ == StaticLib:
      self.sources.append(source)
    elif source.__class__ == Source:
      # FIXME: factor
      p = source.name_relative.with_extension('o')
      if str(p) in drake.Drake.current.nodes:
        o = drake.Drake.current.nodes[p]
      else:
        o = Object(source, tk, cfg)
      self.sources.append(o)
    elif source.__class__ == ResourceFile:
      self.sources.append(Resource(source, tk, cfg))
    elif source.__class__ == Header:
      pass
    elif isinstance(source, (DynLib, Module)):
      self.__dynamic_libraries.add(source)
    else:
      for consumer in source.consumers:
        if isinstance(consumer, drake.Converter) \
           and consumer.source is source:
          try:
            self.src_add(consumer.target, tk, cfg)
            return
          except Exception:
            pass
      for hook in tk.hook_bin_src():
        res = hook(source)
        if res is not None:
          self.src_add(res, tk, cfg)
          return
      raise Exception('invalid source for %s: %s' % (self, source))

  def dependency_add(self, dependency):
    if dependency not in self.dependencies:
      if isinstance(dependency, DynLib):
        self.__dynamic_libraries.add(dependency)
      super().dependency_add(dependency)

class Library(Binary):
  pass

class DynLib(Library):
  def __init__(self, path, sources = None, tk = None, cfg = None,
               preserve_filename = False, strip = False):
    path = Path(path)
    if not preserve_filename and tk is not None:
      path = tk.libname_dyn(path, cfg)
    Binary.__init__(self, path, sources, tk, cfg)
    for lib in self.dynamic_libraries:
      self.dependency_add(lib)
    if tk is not None and cfg is not None and sources is not None:
      DynLibLinker(self, self.tk, self.cfg, strip = strip)

  def clone(self, path):
    res = DynLib(path, None, self.tk, self.cfg,
                 preserve_filename = True)
    return res

  @property
  def install_command(self):
    import platform
    if platform.system() == 'Darwin':
      return ['install_name_tool', '-id', self.path(), self.path()]

Node.extensions['dll'] = DynLib
Node.extensions['dylib'] = DynLib
Node.extensions['so'] = DynLib


class Module(Library):

  # FIXME: Factor with DynLib
  def __init__(self, path, sources = None, tk = None, cfg = None,
               preserve_filename = False):
    path = Path(path)
    if not preserve_filename and tk is not None:
      path = tk.libname_module(cfg, path)
    Binary.__init__(self, path, sources, tk, cfg)
    for lib in self.dynamic_libraries:
      self.dependency_add(lib)
    if tk is not None and cfg is not None and sources is not None:
      DynLibLinker(self, self.tk, self.cfg)

  # FIXME: Factor with DynLib.clone
  def clone(self, path):
    res = Module(path, None, self.tk, self.cfg,
                 preserve_filename = True)
    return res


class StaticLib(Library):

  def __init__(self, path, sources = None, tk = None, cfg = None):
    if tk is not None:
      path = tk.libname_static(cfg, path)
    Binary.__init__(self, path, sources, tk, cfg)
    if sources is not None:
      assert tk is not None
      assert cfg is not None
      StaticLibLinker(self.sources, self, self.tk, self.cfg)


Node.extensions['a'] = StaticLib


class Executable(Binary):

  def __init__(self, path, sources = None, tk = None, cfg = None,
               strip = False):
    if tk is not None:
      path = tk.exename(cfg, path)
    Binary.__init__(self, path, sources, tk, cfg)
    for lib in self.dynamic_libraries:
      self.dependency_add(lib)
    if sources is not None:
      Linker(self, self.tk, self.cfg, strip = strip)


def deps_merge(nodes):
  def merge(d, into):
    for k in d:
      if k not in into:
        into[k] = d[k]
      else:
        merge(d[k], into[k])
  res = {}
  for node in nodes:
    merge(node.mkdeps(), res)
  return res


def all_objects_if_none(nodes):
  if len(nodes):
    return nodes
  else:
    return [node for node in drake.Drake.current.nodes.values()
            if node.__class__ == Object]

def dot_merge(nodes):

    def rec(n, d, marks = {}, skip = False):
        if not skip:
            print('  node_%s [label="%s"]' % (n.uid, n.name()))
        for s in d:
            rec(s, d[s], marks)
            k = (n.uid, s.uid)
            if k not in marks:
                marks[k] = None
                print('  node_%s -> node_%s' % k)

    print('digraph')
    print('{')
    deps = deps_merge(all_objects_if_none(nodes))
    marks = {}
    print('  {')
    print('    rank=same')
    for n in deps:
        print('    node_%s [label="%s"]' % (n.uid, n.name()))
    print('  }')
    for n in deps:
        rec(n, deps[n], marks, True)
    print('}')

def dot_spread(nodes):

    def rec(n, d, i = 0):
        me = i
        print('  node_%s [label="%s"]' % (me, n.name()))
        for s in d:
            si = i + 1
            i = rec(s, d[s], si)
            k = (s.uid, n.uid)
            print('  node_%s -> node_%s' % (me, si))
        return i


    print('digraph')
    print('{')
    deps = deps_merge(all_objects_if_none(nodes))
    i = 0
    for n in deps:
        i = rec(n, deps[n], i) + 1
    print('}')

command_add('cxx-deps-dot-merge', dot_merge)
command_add('cxx-deps-dot', dot_spread)

def find_library(token = None,
                 name = None,
                 prefix = None,
                 include_dir = None,
                 libs = None,
                 toolkit = None):
  if prefix is None or isinstance(prefix, (str, Path)):
    conf = LibraryConfiguration(token = token,
                                name = name,
                                prefix = prefix,
                                include_dir = include_dir,
                                libs = libs,
                                toolkit = toolkit)
    return conf.config()
  else:
    return prefix

class PkgConfig():

    try:
      subprocess.check_output(['which', 'pkg-config'])
      available = True
    except:
      available = False

    def __init__(self, package, version = None):
      self.__package = package
      self.__include_path = None
      self.__library_path = None
      self.__library = None
      self.__exists = None
      self.__prefix = None
      self.__version = None

    @property
    def exists(self):
      if self.__exists is None:
        try:
          self.__pkg_config([])
          self.__exists = True
        except:
          self.__exists = False
      return self.__exists

    def __pkg_config(self, cmd):
      base = ['pkg-config', self.__package]
      if self.__version is not None:
        base += ['--exact-version', str(self.__version)]
      output = subprocess.check_output(base + cmd)
      return output.decode('utf-8').strip().split()

    def __flags(self, cmd, expected):
      res = []
      for flag in self.__pkg_config(cmd):
        if not flag.startswith(expected):
          raise Exception('pkg-config %s gave a strange flag: %s' % \
                          (' '.join(cmd), flag))
        res.append(flag[len(expected):])
      return res

    @property
    def include_path(self):
      if self.__include_path is None:
        self.__include_path = []
        for path in self.__flags(['--cflags-only-I'], '-I'):
          self.__include_path.append(drake.Path(path))
      return self.__include_path

    @property
    def library_path(self):
      if self.__library_path is None:
        self.__library_path = []
        for path in self.__flags(['--libs-only-L'], '-L'):
          self.__library_path.append(drake.Path(path))
      return self.__library_path

    @property
    def library(self):
      if self.__library is None:
        self.__library = []
        for path in self.__flags(['--libs-only-l'], '-l'):
          self.__library.append(drake.Path(path))
      return self.__library

    @property
    def prefix(self):
      if self.__prefix is None:
        self.__prefix = self.__pkg_config(['--variable=prefix'])[0]
      return self.__prefix


class LibraryConfiguration(drake.Configuration):

  """Configuration for a classical C/C++ library."""

  def __init__(self, token = None, name = None, prefix = None,
               include_dir = None, libs = None,
               toolkit = None):
    """Find and create a configuration for the library.

    prefix -- Where to find the library.
    token --  Which file to look for (typically, the main header).
    """
    self.__config = drake.cxx.Config()
    # Search the library with pkg-config
    if PkgConfig.available and name is not None:
      pkg_config = PkgConfig(name)
      if pkg_config.exists:
        for include_path in pkg_config.include_path:
          self.__config.add_system_include_path(include_path)
          self.__prefix_symbolic = include_path / '..'
          self.__prefix = self.__prefix_symbolic
        self.__libraries_path = []
        for library_path in pkg_config.library_path:
          self.__config.lib_path(library_path)
          self.__libraries_path.append(library_path)
        for library in pkg_config.library:
          self.__config.lib(library)
        return
    # Search the library manually.
    if token is not None:
      include_dir = include_dir or 'include'
      if not isinstance(include_dir, list):
        include_dir = [include_dir]
      include_dir = [drake.Path(p) for p in include_dir]
      # Make prefix absolute wrt the source dir
      prefix_symbolic = None
      if prefix is not None:
        prefix_symbolic = drake.Path(prefix)
        prefix = drake.Path(prefix)
        if not prefix.absolute():
          prefix = drake.path_source(prefix)
      # Compute the search path.
      if prefix is None:
        test = [Path('/usr'), Path('/usr/local')]
      else:
        test = [Path(prefix)]
      prefix, include_dir = self._search_many_all(
          [p / token for p in include_dir], test)[0]
      include_dir = include_dir.without_suffix(token)
      include_path = prefix / include_dir
      if not include_path.absolute():
        include_path = include_path.without_prefix(drake.path_source())
      self.__prefix = prefix
      self.__config.add_system_include_path(include_path)
      self.__prefix_symbolic = prefix_symbolic or self.__prefix
      self.__libraries_path = self.__prefix_symbolic / 'lib'
      self.__config.lib_path(self.__prefix / 'lib')
    if libs is None:
      libs = ()
    for choices in libs:
      if not isinstance(choices, tuple):
        choices = (choices,)
      where, lib = self._search_any(
        [(choice, toolkit.libname_dyn(choice)) for choice in choices],
        (prefix / 'lib',))
      self.__config.lib(lib)
    self.__config.prefix = self.__prefix
    self.__config.include_dir = include_dir

  def config(self):
    return self.__config

  def __repr__(self):
    return '%s(prefix = %s)' % (self.__class__, repr(self.__prefix))

  @property
  def prefix(self):
    return self.__prefix_symbolic

  @property
  def libraries_path(self):
    return self.__libraries_path

class CompilationDatabase(drake.Node):

  class Builder(drake.Builder):

    def __init__(self, database):
      super().__init__([], [database])
      self.__database = database

    def execute(self):
      res = []
      self.output('Generate %s' % self.__database)
      for node in drake.Drake.current.nodes.values():
        if isinstance(node, drake.cxx.Object) and node.builder is not None:
          res.append({
            'directory': str(drake.path_build(absolute = True)),
            'command': drake.command_flatten(node.builder.command),
            'file': str(node.path()),
          })
      with open(str(self.__database.path()), 'w') as f:
        import json
        json.dump(res, f)
      return True

  def __init__(self, path = None):
    super().__init__(path)
    CompilationDatabase.Builder(self)

class PatchAndInstall(drake.Install):
  def execute(self):
    if not super.execute():
      return False
    output = subprocess.check_output(['otool', '-L', self.target.path()])
    lines = output.decode('utf-8').strip().split('\n')
    lines = [x.split(' ')[0] for x in lines if 'libstdc++' in x or 'libgcc_s.1' in x]
    for l in lines:
      self.cmd('Localize libstdc++ of %s' % self.target,
        ['install_name_tool', '-change', l, '@rpath/%s' % (os.path.filename(l))], throw = True)
