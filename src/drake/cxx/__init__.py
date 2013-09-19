# Copyright (C) 2009-2012, Quentin "mefyl" Hocquet
#
# This software is provided "as is" without warranty of any kind,
# either expressed or implied, including but not limited to the
# implied warranties of fitness for a particular purpose.
#
# See the LICENSE file for more information.

import collections
import drake
import re
import shutil
import subprocess
import sys
import tempfile

_OS = __import__('os')
from .. import ShellCommand, Builder, Node, Path, node, prefix, srctree, Exception, arch, os, cmd, command_add, debug, Expander, FileExpander
from .. import utils
from copy import deepcopy
from .. import sched

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
            self.flags = []
            self._framework = {}
            self.__defines = collections.OrderedDict()
            self.__standard = None
            self.__rpath = []
            self.__warnings = Config.Warnings()
        else:
            self.__debug = model.__debug
            self.__export_dynamic = model.__export_dynamic
            self._includes = deepcopy(model._includes)
            self.__local_includes = sched.OrderedSet(model.__local_includes)
            self.__optimization = model.__optimization
            self.__system_includes = sched.OrderedSet(model.__system_includes)
            self.__lib_paths = sched.OrderedSet(model.__lib_paths)
            self.__libs = sched.OrderedSet(model.__libs)
            self.flags = deepcopy(model.flags)
            self._framework = deepcopy(model._framework)
            self.__defines = collections.OrderedDict(model.__defines)
            self.__standard = model.__standard
            self.__rpath = deepcopy(model.__rpath)
            self.__warnings = Config.Warnings(model.__warnings)

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

    def framework_add(self, name):

        self._framework[name] = None

    def frameworks(self):

        return self._framework.keys()

    def add_local_include_path(self, path):

        path = prefix() / Path(path)
        self.__local_includes.add(path)
        self._includes[path] = None


    def add_system_include_path(self, path):
        path = Path(path)
        # Never include those.
        # FIXME: Unix only
        if path == Path('/include') or path == Path('/usr/include'):
            return
        if not path.absolute():
            path = srctree() / prefix() / path
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


    def lib_path(self, path):

        if path == Path('/lib') or path == Path('/usr/lib'):
            return
        p = Path(path)
        # if not p.absolute():
        #     p = srctree() / prefix() / p
        if not p.absolute():
            p = prefix() / p
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
        drake.Exception: redefinition of B from 0 to 1
        """
        def merge_bool(attr):
          mine = getattr(self, attr)
          hers = getattr(rhs, attr)
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

        for key, value in rhs.__defines.items():
          if key in res.__defines:
            old = res.__defines[key]
            if old != value:
              raise Exception('redefinition of %s from %s to %s' % \
                              (key, old, value))
          else:
            res.__defines[key] = value

        res.__local_includes.update(rhs.__local_includes)
        res.__system_includes.update(rhs.__system_includes)
        res._includes.update(rhs._includes)
        res._framework.update(rhs._framework)
        res.__lib_paths.update(rhs.__lib_paths)
        res.__libs.update(rhs.__libs)
        res.flags += rhs.flags
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

  def __init__(self, compiler = 'g++', compiler_c = 'gcc', os = None):
    Toolkit.__init__(self)
    self.arch = arch.x86
    self.os = os
    try:
      version = drake.cmd_output([compiler, '--version'])
    except:
      raise drake.Exception('Unable to find compiler: %s' % compiler)
    self.cxx = compiler
    self.c = compiler_c
    if self.os is None:
      osx = '__APPLE__'
      if self.preprocess(osx).strip().split('\n')[-1] != osx:
        self.os = drake.os.macos
      else:
        self.os = drake.os.linux

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
      for include in cfg.local_include_path:
          res.append('-I')
          res.append(utils.shell_escape(srctree() / include))
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
      else:
          raise Exception('Unknown C++ standard: %s' % std)
      if cfg.warnings:
          res.append('-Wall')
      for warning, enable in cfg.warnings:
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
      if cfg.export_dynamic and self.os is not drake.os.macos:
          res.append('-rdynamic')
      return res

  def compile(self, cfg, src, obj, c = False, pic = False):
      extraflags = []
      if pic:
          extraflags.append('-fPIC')
      return [c and self.c or self.cxx] + cfg.flags + \
          self.cppflags(cfg) + self.cflags(cfg) + \
          extraflags + ['-c', str(src), '-o', str(obj)]

  def archive(self, cfg, objs, lib):
      # FIXME: ;
      return (['ar', 'crs', str(lib.path())] + \
                  list(map(lambda n: str(n.path()), objs)),
              ['ranlib', str(lib.path())])

  def __libraries_flags(self, cfg, cmd):
    for lib in cfg.libs_dynamic:
      cmd.append('-l%s' % lib)
    # XXX Should refer to libraries with path on MacOS.
    if cfg.libs_static:
      if self.os is not drake.os.macos:
        cmd.append('-Wl,-Bstatic')
        for lib in cfg.libs_static:
          cmd.append('-l%s' % lib)
        cmd.append('-Wl,-Bdynamic')
      else:
        # This is some kind of joke from Apple, -Bstatic is a
        # no-op on Lion.
        for lib in cfg.libs_static:
          found = False
          for path in cfg.library_path:
            libpath = path / ('lib%s.a' % lib)
            if libpath.exists():
              cmd.append(libpath)
              found = True
              break
          if not found:
            raise Exception('can\t find static version of %s' % lib)


  def link(self, cfg, objs, exe):
      cmd = [self.cxx] + self.ldflags(cfg)
      for framework in cfg.frameworks():
          cmd += ['-framework', framework]
      for path in cfg.library_path:
          cmd += ['-L', path]
          if self.os == drake.os.linux:
              cmd.append('-Wl,-rpath-link,%s' % path)
      for path in cfg._Config__rpath:
        cmd.append('-Wl,-rpath,%s' % self.rpath(path))
      if self.os == drake.os.macos:
          cmd += ['-undefined', 'dynamic_lookup']
      for obj in objs:
          cmd.append(obj.path())
      cmd += ['-o', exe.path()]
      self.__libraries_flags(cfg, cmd)
      return cmd

  def dynlink(self, cfg, objs, exe):
      cmd = [self.cxx] + cfg.flags + self.ldflags(cfg)
      for framework in cfg.frameworks():
          cmd += ['-framework', framework]
      for path in cfg.library_path:
          cmd += ['-L', path]
      for path in cfg._Config__rpath:
        cmd.append('-Wl,-rpath,%s' % self.rpath(path))
      if self.os == drake.os.macos:
        cmd += ['-undefined', 'dynamic_lookup',
                '-Wl,-install_name,@rpath/%s' % exe.path().basename(),
                '-Wl,-headerpad_max_install_names']
      cmd.append('-Wl,-soname,%s' % exe.name().basename())
      for obj in objs:
          cmd.append(obj.path())
      cmd += ['-shared', '-o', exe.path()]
      self.__libraries_flags(cfg, cmd)
      return cmd

  def libname_static(self, cfg, path):

      path = Path(path)
      return path.dirname() / ('lib%s.a' % str(path.basename()))

  def libname_dyn(self, cfg, path):

      path = Path(path)
      if self.os == drake.os.linux:
          ext = 'so'
      elif self.os == drake.os.macos:
          ext = 'dylib'
      else:
          assert False
      return path.dirname() / ('lib%s.%s' % (path.basename(), ext))

  def libname_module(self, cfg, path):

      path = Path(path)
      return path.dirname() / ('%s.so' % str(path.basename()))

  def exename(self, cfg, path):

      return Path(path)

  def rpath_set_command(self, binary, path):
    path = self.rpath(path)
    if self.os is drake.os.macos:
      return ['install_name_tool',
              '-add_rpath', str(path),
              str(binary)]
    else:
      return ['patchelf',
              '--set-rpath', str(path),
              str(binary)]

  def rpath(self, path):
    path = drake.Path(path)
    if not path.absolute():
      if self.os is drake.os.macos:
        return drake.Path('@loader_path') / path
      else:
        return drake.Path('$ORIGIN') / path
    else:
      return path


class VisualToolkit(Toolkit):

  arch = arch.x86
  os = os.windows

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
    _OS.environ['PATH'] = ';'.join(path)
    _OS.environ['LIB'] = lib
    if override_include is not None:
      include = [override_include] + include
    _OS.environ['INCLUDE'] = ';'.join(include)
    shutil.rmtree(str(tmp))
    self.flags = []

  def preprocess(self, code, config = Config()):

    fd, path = tempfile.mkstemp()
    try:
      with _OS.fdopen(fd, 'w') as f:
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
      _OS.remove(path)

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
      flags.append('/I%s' % utils.shell_escape(srctree() / path))
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

  def libname_dyn(self, cfg, path):
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


def mkdeps(res, n, lvl, config, marks,
         f_submarks, f_init, f_add):
  include_re = re.compile('\\s*#\\s*include\\s*(<|")(.*)(>|")')
  path = n.path()
  idt = ' ' * lvl * 2
  if str(path) in marks:
    return
  marks[str(path)] = True
  # debug.debug('%smkdeps: %s' % (idt, path))

  f_init(res, n)

  def unique(include, prev_via, prev, new_via, new):
    if prev is not None:
      raise Exception('two nodes match inclusion %s: '\
                          '%s via %s and %s via %s' % \
                          (include, prev, prev_via, new, new_via))
    return new, new_via

  n.build()
  matches = []
  with open(str(path), 'rb') as include_file:
    for line in include_file:
      line = line.strip()
      match = include_re.match(line.decode('utf-8'))
      if match:
        matches.append(match)

  for match in matches:
    include = match.group(2)
    search = set(config.local_include_path)
    if match.group(1) == '"':
      search.add(n.name().dirname())
    found = None
    via = None
    for include_path in search:
      name = include_path / include
      test = name
      if str(test) in Node.nodes:
        # Check this is not an old cached dependency from
        # cxx.inclusions. Not sure of myself though.
        if test.is_file() or node(str(test)).builder is not None:
          found, via = unique(include, via, found, test, node(test))
          # debug.debug('%sfound node: %s' % (idt, test))
      if not found or srctree() != Path('.'):
        test = srctree() / test
        if test.is_file():
          # debug.debug('%sfound file: %s' % (idt, test))
          found, via = unique(include, via, found, test,
                              node(name, Header))
    if found is not None:
      rec = []
      mkdeps(rec, found, lvl + 1, config,
             f_submarks(marks), f_submarks, f_init, f_add)
      f_add(res, found, rec)
    else:
      debug.debug('%sinclusion not found: %s (search path: %s)'\
                  % (idt, include, ', '.join(map(str, search))))

class Compiler(Builder):

  name = 'C++ compilation'
  deps = 'drake.cxx.inclusions'

  Builder.register_deps_handler(deps, deps_handler)

  def __init__(self, src, obj, tk, cfg, c = False):
    self.src = src
    self.obj = obj
    self.config = cfg
    self.toolkit = tk
    self.__c = c
    Builder.__init__(self, [src], [obj])


  def dependencies(self):
    deps = []
    mkdeps(deps, self.src, 0, self.config, {},
           f_init = lambda res, n: res.append(n),
           f_submarks = lambda d: d,
           f_add = lambda res, node, sub: res.extend(sub))

    for dep in deps:
      if dep != self.src:
        self.add_dynsrc(self.deps, dep)
    for hook in self.toolkit.hook_object_deps():
      hook(self)

  def execute(self):
    # FIXME: handle this in the toolkit itself.
    leave_stdout = isinstance(self.toolkit, VisualToolkit)
    return self.cmd('Compile %s' % self.obj, self.command,
                    leave_stdout = leave_stdout)

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
        if isinstance(consumer, DynLibLinker):
          return True
        elif isinstance(consumer, StaticLibLinker):
          if pic_rec(consumer.library):
            return True
      return False
    return pic_rec(self.obj)

  def hash(self):
    return str(self.command)

  def mkdeps(self):
    def add(res, node, sub):
      res[node] = sub
    return {self.src: mkdeps(self.src, 0, self.config, {},
                             f_init = lambda n: {},
                             f_submarks = lambda d: dict(d),
                             f_add = add)}

  def __str__(self):
    return 'Compiler(%s)' % self.obj

  def __repr__(self):
    return 'Compiler(%s)' % self.obj

class Linker(Builder):

  name = 'executable linkage'

  def hash(self):
    return str(self.command)

  def dependencies(self):
    for hook in self.toolkit.hook_bin_deps():
      hook(self)

  def __init__(self, exe, tk, cfg):
    # FIXME: factor with DynLibLinker
    self.exe = exe
    self.toolkit = tk
    self.config = drake.cxx.Config(cfg)
    Builder.__init__(self, exe.sources + exe.dynamic_libraries, [exe])

  def execute(self):
    return self.cmd('Link %s' % self.exe, self.command)

  @property
  def command(self):
    objects = self.exe.sources + self.exe.dynamic_libraries
    return self.toolkit.link(
        self.config,
        objects + list(self.sources_dynamic()),
        self.exe)

  def __repr__(self):
    return 'Linker(%s)' % self.exe

  def __str__(self):
    return 'Linker(%s)' % self.exe


class DynLibLinker(Builder):

  name = 'dynamic library linkage'

  def dependencies(self):
    for hook in self.toolkit.hook_bin_deps():
      hook(self)

  def __init__(self, lib, tk, cfg):
    self.lib = lib
    self.toolkit = tk
    self.config = Config(cfg)
    self.__library = lib
    # This duplicates self.__sources, but preserves the order.
    self.__objects = lib.sources
    self.__dynamic_libraries = lib.dynamic_libraries
    Builder.__init__(self,
                     self.__objects + lib.dynamic_libraries,
                     [lib])

  def execute(self):
    return self.cmd('Link %s' % self.lib, self.command)

  @property
  def command(self):
    objects = self.__objects + self.__dynamic_libraries
    return self.toolkit.dynlink(
        self.config,
        objects + list(self.sources_dynamic()),
        self.lib)

  def __repr__(self):
    return 'Linker for %s' % self.lib

  def hash(self):
    return repr(self.command)

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
        self.toolkit = tk
        self.config = cfg
        Builder.__init__(self, objs, [lib])

    def execute(self):
        return self.cmd('Link %s' % self.__library, self.command)

    @property
    def command(self):
        return self.toolkit.archive(
            self.config,
            self.objs + list(self.sources_dynamic()),
            self.__library)

    @property
    def library(self):
      return self.__library

    def hash(self):
        return str(self.command)


class Source(Node):

    def __init__(self, path):

        Node.__init__(self, path)

    def clone(self, path):

        return Source(path)

Node.extensions['c'] = Source
Node.extensions['cc'] = Source
Node.extensions['cpp'] = Source
Node.extensions['S'] = Source

class Header(Node):

    def __init__(self, path):

        Node.__init__(self, path)

Node.extensions['h'] = Header
Node.extensions['hh'] = Header
Node.extensions['hpp'] = Header
Node.extensions['hxx'] = Header


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
        path = Path(source.name())
        c = path.extension == 'c'
        path.extension_strip_last_component()
        if len(path.extension):
            path.extension += '.%s' % tk.object_extension()
        else:
            path.extension = tk.object_extension()
        Node.__init__(self, path)

        Compiler(source, self, tk, cfg, c = c)

    def mkdeps(self):

        return self.builder.mkdeps()

Node.extensions['o'] = Object

class Binary(Node):

    def __init__(self, path, sources, tk, cfg):

#         if len(args) == 0:
#             Node.__init__(self, path)
#             self.builder = None
#             return

        self.tk = tk
        self.cfg = cfg

        Node.__init__(self, path)

        self.sources = []
        self.__dynamic_libraries = []

        if sources is not None:
            for source in sources:
                self.src_add(source, self.tk, self.cfg)


    def clone(self, path):

        return Binary(path, self.sources, self.tk, self.cfg)

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
            p = Path(source.name())
            p.extension = 'o'
            if str(p) in Node.nodes:
                o = Node.nodes[p]
            else:
                o = Object(source, tk, cfg)
            self.sources.append(o)
        elif source.__class__ == Header:
            pass
        elif isinstance(source, DynLib):
            self.__dynamic_libraries.append(source)
        else:
            for consumer in source.consumers:
                if isinstance(consumer, Expander):
                    try:
                        self.src_add(consumer.target(), tk, cfg)
                        return
                    except Exception:
                        pass
            for hook in tk.hook_bin_src():
                res = hook(source)
                if res is not None:
                    self.src_add(res, tk, cfg)
                    return
            # obj = source.produced_direct()
            # if obj is None:
            raise Exception('invalid source for a library: %s' % source)
            # self.src_add(obj, tk, cfg)


class DynLib(Binary):
  # FIXME: If path and toolkit are given, deduce lib_name
  def __init__(self, path, sources = None, tk = None, cfg = None,
               preserve_filename = False, lib_name = None):
    path = Path(path)
    if tk is None:
      assert lib_name is not None
      self.__lib_name = lib_name
    else:
      self.__lib_name = str(path.basename())
    if not preserve_filename and tk is not None:
      path = tk.libname_dyn(cfg, path)
    Binary.__init__(self, path, sources, tk, cfg)
    if tk is not None and cfg is not None:
      DynLibLinker(self, self.tk, self.cfg)

  def clone(self, path):
    path = Path(path)
    res = DynLib(path, self.sources, self.tk, self.cfg,
                 preserve_filename = True, lib_name = self.lib_name)
    res.__lib_name = self.__lib_name
    return res

  @property
  def lib_name(self):
    return self.__lib_name

  @property
  def install_command(self):
    if self.tk.os is drake.os.macos:
      return ['install_name_tool', '-id', self.path(), self.path()]

Node.extensions['so'] = DynLib
Node.extensions['dylib'] = DynLib


class Module(Binary):

  def __init__(self, path, sources, tk, cfg):
    Binary.__init__(self, tk.libname_module(cfg, path),
                    sources, tk, cfg)
    DynLibLinker(self, self.tk, self.cfg)


class StaticLib(Binary):

  def __init__(self, path, sources, tk, cfg):
    Binary.__init__(self, tk.libname_static(cfg, path),
                    sources, tk, cfg)
    StaticLibLinker(self.sources, self, self.tk, self.cfg)


class Executable(Binary):

  def __init__(self, path, sources, tk, cfg):
    path = tk.exename(cfg, path)
    Binary.__init__(self, path, sources, tk, cfg)
    Linker(self, self.tk, self.cfg)


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
    return [node for node in Node.nodes.values()
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

def find_library(token = None, name = None, prefix = None,
                 include_dir = None):
    if prefix is None or isinstance(prefix, str):
        conf = LibraryConfiguration(token = token,
                                    name = None,
                                    prefix = prefix,
                                    include_dir = include_dir)
        return conf.config()
    else:
        return prefix

class PkgConfig():

    try:
        drake.cmd_output(['which', 'pkg-config'])
        available = True
    except:
        available = False

    def __init__(self, package):
        self.__package = package
        self.__include_path = None
        self.__library_path = None
        self.__exists = None

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
      output = drake.cmd_output(['pkg-config', self.__package] + cmd)
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

class LibraryConfiguration(drake.Configuration):

  """Configuration for a classical C/C++ library."""

  def __init__(self, token = None, name = None, prefix = None,
               include_dir = None, libs = []):
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
        for library_path in pkg_config.library_path:
          self.__config.lib_path(library_path)
          self.__libraries_path = library_path
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
          prefix = drake.path_src(prefix)
      # Compute the search path.
      if prefix is None:
        test = [Path('/usr'), Path('/usr/local')]
      else:
        test = [Path(prefix)]
      prefix, include_dir = self._search_many_all(
          [p / token for p in include_dir], test)[0]
      include_dir.strip_suffix(token)
      include_path = prefix / include_dir
      if not include_path.absolute():
        include_path.strip_prefix(srctree())
      self.__prefix = prefix
      self.__config.add_system_include_path(include_path)
      self.__prefix_symbolic = prefix_symbolic or self.__prefix
      self.__libraries_path = self.__prefix_symbolic / 'lib'
      self.__config.lib_path(self.__prefix / 'lib')
    for lib in libs:
      self.__config.lib(lib)

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
