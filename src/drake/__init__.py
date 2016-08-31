# Copyright (C) 2009-2016, Quentin "mefyl" Hocquet
#
# This software is provided "as is" without warranty of any kind,
# either expressed or implied, including but not limited to the
# implied warranties of fitness for a particular purpose.
#
# See the LICENSE file for more information.

_OS = __import__('os')
import atexit
import contextlib
import drake.debug
import hashlib
import inspect
import itertools
import os.path
import pickle
import pipes
import platform
import re
import shutil
import stat
import string
import subprocess
import sys
import threading
import time
import types

from drake.sched import Coroutine, Scheduler
from drake.enumeration import Enumerated
from itertools import chain

from drake.sched import logger
from drake.utils import property_memoize

def _scheduled():
  return Coroutine.current and \
    Coroutine.current._Coroutine__scheduler

def path_source(path = None):
  if path is None:
    return Drake.current.path_source
  path = Path(path)
  if path.absolute():
    return path
  else:
    return Drake.current.path_source / Drake.current.prefix / path

PROFILE = 'DRAKE_PROFILE' in os.environ

class Drake:

  current = None

  @property
  def jobs(self):
    return self.__jobs

  def jobs_set(self, n):
    n = int(n)
    if n == 1:
      self.__jobs_lock = None
    else:
      self.__jobs_lock = sched.Semaphore(n)
  jobs.setter(jobs_set)

  @property
  def path_source(self):
    """The path to the source directory relative to the build directory."""
    return self.__source

  @property
  def configure(self):
    return self.__configure

  @property
  def scheduler(self):
    return self.__scheduler

  @property
  def jobs_lock(self):
    return self.__jobs_lock

  __previous = []
  def __enter__(self):
    Drake.__previous.append(Drake.current)
    Drake.current = self
    return self

  def __exit__(self, *args):
    Drake.current = Drake.__previous[-1]
    del Drake.__previous[-1]

  @property
  def prefix(self):
    """The current prefix.

    The prefix is the path from the root of the build tree to the
    current drakefile build tree. This is '.' for the root drakefile.
    """
    return self.__prefix

  @property
  def nodes(self):
    return self.__nodes

  def recurse(self, path):
    class Recurser:
      def __init__(self, drake):
        self.__drake = drake
        self.__previous = None
      def __enter__(self):
        self.__previous = self.__drake.prefix
        self.__drake._Drake__prefix = self.__previous / path
      def __exit__(self, *args, **kwargs):
        self.__drake._Drake__prefix = self.__previous
    return Recurser(self)

  def __init__(self,
               root = None,
               jobs = None,
               kill_builders_on_failure = False,
  ):
    if root is None:
      root = drake.Path('.')
    self.__jobs = 1
    self.__jobs_lock = None
    self.__kill_builders_on_failure = kill_builders_on_failure
    self.__source = drake.Path(root)
    self.__scheduler = Scheduler(policy = sched.DepthFirst())
    self.__prefix = drake.Path('.')
    self.__nodes = {}
    # Load the root drakefile
    self.__globals = {}
    path = str(self.path_source / 'drakefile')
    if _OS.path.exists(path):
      with open(path, 'r') as drakefile:
        with self:
          exec(compile(drakefile.read(), path, 'exec'),
               self.__globals)
      self.__module = _Module(self.__globals)
      self.__configure = self.__module.configure
    else:
      self.__module = _Module({})
      self.__configure = None
    if jobs is not None:
      self.jobs_set(jobs)

  @property
  def kill_builders_on_failure(self):
    return self.__kill_builders_on_failure

  def run(self, *cfg, **kwcfg):
    try:
      g = self.__globals
      module = self.__module
      configure = self.__configure
      # Parse arguments
      options = {
        '--jobs': lambda j: self.jobs_set(j),
        '-j'    : lambda j : self.jobs_set(j),
        '--help': help,
        '-h'    : help,
        '--complete-modes': complete_modes,
        '--complete-options': complete_options,
        '--complete-nodes': complete_nodes,
      }
      arguments_re = re.compile('--(\\w+)=(.*)')
      specs = inspect.getfullargspec(configure)
      callbacks = []
      i = 0
      args = sys.argv[1:]
      while i < len(args):
        match = arguments_re.match(args[i])
        if match:
          name = match.group(1)
          value = match.group(2)
          if name in specs.args:
            if name in specs.annotations:
              t = specs.annotations[name]
              if t is bool:
                if value.lower() in ['true', 'yes']:
                  value = True
                elif value.lower() in ['false', 'no']:
                  value = False
                else:
                  raise Exception('invalid value for '
                                  'boolean option %s: %s' % (name, value))
            kwcfg[name] = value
            del args[i]
            continue
        elif args[i] in options:
          opt = args[i]
          del args[i]
          opt_args = []
          for a in inspect.getfullargspec(options[opt]).args:
            opt_args.append(args[i])
            del args[i]
          cb = options[opt](*opt_args)
          if cb is not None:
            callbacks.append(cb)
          continue
        i += 1
      # Configure
      with self:
        configure(*cfg, **kwcfg)
      # Run callbacks.
      for cb in callbacks:
        cb()
      mode = _MODES['build']
      i = 0
      with self:
        while True:
          if i < len(args):
            arg = args[i]
            if arg[0:2] == '--':
              arg = arg[2:]
              if arg in _MODES:
                mode = _MODES[arg]
              else:
                raise Exception('Unknown option: %s.' % arg)
              i += 1
          nodes = []
          while i < len(args) and args[i][0:2] != '--':
            nodes.append(node(args[i]))
            i += 1
          if not nodes:
            nodes = _DEFAULTS
          mode(nodes)
          if i == len(args):
            break
    except Exception as e:
      print('%s: %s' % (sys.argv[0], e))
      if 'DRAKE_DEBUG_BACKTRACE' in _OS.environ:
        import traceback
        traceback.print_exc()
      exit(1)
    except KeyboardInterrupt:
      print('%s: interrupted.' % sys.argv[0])
      exit(1)

EXPLAIN = 'DRAKE_EXPLAIN' in _OS.environ
def explain(node, reason):
  if EXPLAIN:
    print('Execute %s because %s' % (node, reason))

def warn(msg):
  print('Warning: %s.' % msg, file = sys.stderr)

class Profile:

  def __init__(self, name):
    self.__calls = 0
    self.__name = name
    self.__time = 0
    if PROFILE:
      atexit.register(self.show)

  def __call__(self):
    if PROFILE:
      return ProfileInstance(self)
    else:
      return drake.log.NOOP

  def show(self):
    print(self)

  def __str__(self):
    return '%s: called %s time, %s seconds.' % (
      self.__name, self.__calls, self.__time)

class ProfileInstance:

  def __init__(self, parent):
    self.__parent = parent
    self.__time  = 0
    self.__time  = None

  def __enter__(self):
    self.__time = time.time()

  def __exit__(self, *args):
    self.__parent._Profile__calls += 1
    t = time.time() - self.__time
    self.__parent._Profile__time  += t


profile_hashing = Profile('files hashing')
profile_unpickling = Profile('dependencies files reading')
profile_pickling = Profile('dependencies files writing')

class NodeRedefinition(Exception):

    """Thrown when a node is redefined.

    In most cases, this exception will not be seen by the user
    since all primitives (even node constructors) will return the
    existing node if it exists."""

    def __init__(self, name_absolute):
        """Build a redefinition exception.

        name_absolute -- the absolute name of the redefined node."""
        Exception.__init__(self)
        self.__name = name_absolute

    def __str__(self):
        """Description, with the redefined node name."""
        return 'node redefinition: %s' % self.__name

    def name(self):
        """The absolute name of the redefined node.

        See Node.name_absolute."""
        return self.__name

class NoBuilder(Exception):

    """Node raised when a Node builder is missing.

    Raised when a Nodehas no builder and its associated file does not
    exist.
    """

    def __init__(self, node):
        """Build a NoBuilder exception.

        node -- the node whose builder is missing.
        """
        self.__node = node
        Exception.__init__(self,
                           'no builder to make %s' % self.__node)


class BuilderRedefinition(Exception):

  """Exception raised when two builders are defined for a node."""

  def __init__(self, node, previous, new):
    self.__node = node
    self.__previous = previous
    self.__new = new
    super().__init__('builder redefinition for %s' % self.__node)

  @property
  def node(self):
    return self.__node

  @property
  def previous_builder(self):
    return self.__previous

  @property
  def new_builder(self):
    return self.__new


_RAW = 'DRAKE_RAW' in _OS.environ
_SILENT = 'DRAKE_SILENT' in _OS.environ

class Path:

  """Node names, similar to a filesystem path."""

  cache = {}

  def __new__(self,
              path,
              absolute = None,
              virtual = None,
              volume = None,
              string = None):
    if path.__class__ is Path:
      return path
    elif path.__class__ is str:
      strkey = path
      res = Path.cache.get(path, None)
      if res is not None:
        return res
      else:
        path, absolute, virtual, volume = Path.__parse(path)
    else:
      strkey = string
    if Path.windows:
      key = (path, absolute, virtual, volume)
    else:
      key = (path, absolute, virtual)
    res = Path.cache.get(key, None)
    if res is None:
      res = object.__new__(self)
      res.__init(path, absolute, virtual, volume)
      Path.cache[key] = res
    if strkey is not None:
      Path.cache[strkey] = res
    return res

  def unfold(self):
    p = self
    while p != drake.Path.dot:
      yield p
      p = p.dirname()

  @classmethod
  def rootify(self, paths):
    res = set()
    for p in paths:
      insert = True
      for e in set(res):
        if e.prefix_of(p):
          insert = False
          break
        elif p.prefix_of(e):
          res.remove(e)
      if insert:
        res.add(p)
    return res

  def __parse(path):
    if Path.windows:
      volume = re.compile('^([a-zA-Z]):').match(path)
      if volume:
        volume = volume.group(1)
        path = path[2:]
      else:
        volume = ''
      if path[:2] == '//' or path[:2] == '\\\\':
        path = path[2:]
        absolute = False
        virtual = True
      elif path[:1] == '/' or path[:1] == '\\':
        path = path[1:]
        absolute = True
        virtual = False
      else:
        absolute = False
        virtual = False
      path = path = tuple(re.split(r'/|\\', path))
    else:
      volume = None
      if path[:2] == '//':
        path = path[2:]
        absolute = False
        virtual = True
      elif path[:1] == '/':
        path = path[1:]
        absolute = True
        virtual = False
      else:
        absolute = False
        virtual = False
      path = tuple(path.split('/'))
    return path, absolute, virtual, volume

  separator = '/'

  windows = platform.system() == 'Windows'
  if windows:
    separator = '\\'

  def __init(self, path, absolute, virtual, volume):
    """Build a path.

    path -- The path, as a string or an other Path.
    """
    self.__absolute = absolute
    self.__canonized = None
    self.__path = path
    self.__str = None
    self.__virtual = virtual
    self.__volume = volume

  def canonize(self):
    if self.__canonized is None:
      res = ()
      path = self.__path
      for i in range(len(path)):
        if path[i] == '..' and len(res) > 0 and res[-1] != '..':
          res = res[:-1]
        elif path[i] == '.':
          pass
        else:
          res += (path[i],)
      if res == self.__path:
        self.__canonized = self
      else:
        self.__canonized = drake.Path(res,
                                      absolute = self.__absolute,
                                      virtual = self.__virtual,
                                      volume = self.__volume)
    return self.__canonized

  def absolute(self):
      """Whether this path is absolute.

      >>> Path('.').absolute()
      False
      >>> Path('foo/bar').absolute()
      False
      >>> Path('/').absolute()
      True
      >>> Path('/foo').absolute()
      True
      """
      return self.__absolute

  @property
  def relative(self):
    return not self.__absolute

  @property
  def virtual(self):
    """Whether this path is virtual.

    >>> Path('.').virtual
    False
    >>> Path('foo/bar').virtual
    False
    >>> Path('//').virtual
    True
    >>> Path('//foo').virtual
    True
    """
    return self.__virtual

  @property
  def volume(self):
    return self.__volume


  def remove(self, err = False):
      """Remove the target file.

      err -- Whether this is an error for non-existent file.

      No-op if the file does not exist, unless err is true.

      >>> p = Path('/tmp/.drake.foo')
      >>> p.touch()
      >>> p.exists()
      True
      >>> p.remove()
      >>> p.exists()
      False
      >>> p.touch()
      >>> p.remove(True)
      >>> p.remove(True)
      Traceback (most recent call last):
          ...
      Exception: Path does not exist: /tmp/.drake.foo
      """
      try:
          _OS.remove(str(self))
      except OSError as e:
        import sys
        if e.errno == 2:
          if err:
            raise Exception('Path does not exist: %s' % str(self))
        # OS X throws an errno 1 when trying to remove a directory.
        elif e.errno == 21 or (sys.platform == 'darwin' and e.errno == 1):
          shutil.rmtree(str(self))
        else:
          raise

  def __extension_get(self):
      parts = self.__path[-1].split('.')
      if len(parts) > 1:
          return '.'.join(parts[1:])
      else:
          return ''

  def with_extension(self, value):
    '''The path with a different extension.

    >>> p = Path('foo')
    >>> p
    Path("foo")
    >>> p = p.with_extension('tar.bz2')
    >>> p
    Path("foo.tar.bz2")
    >>> p.with_extension('txt')
    Path("foo.txt")
    '''
    parts = self.__path[-1].split('.')
    if len(parts) > 1:
      if value == '':
        parts = [parts[0]]
      else:
        parts = [parts[0], value]
      return Path(self.__path[:-1] + ('.'.join(parts),),
                  absolute = self.__absolute,
                  virtual = self.__virtual,
                  volume = self.__volume)
    else:
      if value != '':
        return Path(self.__path[:-1] + ('%s.%s' % (parts[0], value),),
                    absolute = self.__absolute,
                    virtual = self.__virtual,
                    volume = self.__volume)
      else:
        return self

  extension = property(
      fget = __extension_get,
      doc = """Extension of the file name.

      The extension is the part after the first dot of the basename,
      or the empty string if there are no dot.

      >>> Path('foo.txt').extension
      'txt'
      >>> Path('foo.tar.bz2').extension
      'tar.bz2'
      >>> Path('foo').extension
      ''
      """)

  def without_last_extension(self):
      """Remove the last dot and what follows from the basename.

      Does nothing if there is no dot.

      >>> p = Path('foo.tar.bz2')
      >>> p
      Path("foo.tar.bz2")
      >>> p = p.without_last_extension()
      >>> p
      Path("foo.tar")
      >>> p = p.without_last_extension()
      >>> p
      Path("foo")
      >>> p.without_last_extension()
      Path("foo")
      """
      ext = '.'.join(self.extension.split('.')[:-1])
      return self.with_extension(ext)

  def __str__(self):
    """The path as a string, adapted to the underlying OS."""
    if self.__str is None:
      if self.__absolute:
        if self.__volume is None or self.__volume == '':
          prefix = drake.Path.separator
        else:
          prefix = self.__volume + ':' + drake.Path.separator
      elif self.__virtual:
        prefix = drake.Path.separator * 2
      else:
        prefix = ''
      if not self.__path:
        body = '.'
      else:
        body = self.separator.join(self.__path)
      self.__str = prefix + body
    return self.__str

  def __repr__(self):
      """Python representation."""
      return 'Path(\"%s\")' % str(self)

  def __lt__(self, rhs):
      """Arbitrary comparison.

      >>> Path('foo') < Path('foo')
      False
      >>> (Path('foo') < Path('bar')) ^ (Path('bar') < Path('foo'))
      True
      """
      return str(self) < str(rhs)

  def __hash__(self):
    """Hash value.

    >>> hash(Path('foo')) == hash(Path('foo'))
    True
    """
    return id(self)

  def exists(self):
      """Whether the designed file or directory exists.

      >>> p = Path('/tmp/.drake.foo')
      >>> p.touch()
      >>> p.exists()
      True
      >>> p.remove()
      >>> p.exists()
      False
      """
      if _OS.path.islink(str(self)):
          return True
      return _OS.path.exists(str(self))

  @property
  def executable(self):
      """Whether the designed file is executable by the user."""
      return _OS.access(str(self), _OS.X_OK)

  def is_file(self):
      """Whether the designed file exists and is a regular file.

      >>> p = Path('/tmp/.drake.foo')
      >>> p.touch()
      >>> p.is_file()
      True
      >>> p.remove()
      >>> p.is_file()
      False
      >>> p.mkpath()
      >>> p.exists()
      True
      >>> p.is_file()
      False
      >>> p.remove()
      """
      return _OS.path.isfile(str(self))

  def basename(self):
    """The filename part of the path.

    This is the path without the dirname. Throws if the path has
    no components.


    >>> Path('foo/bar/baz').basename()
    Path("baz")
    """
    if not self.__path:
      raise Exception('Cannot take the basename of an empty path.')
    return Path(self.__path[-1:],
                absolute = False,
                virtual = False,
                volume = '')

  def dirname(self):
    """The directory part of the path.

    This is the path without the basename. Throws if the path has
    no components.

    >>> Path('foo/bar/baz').dirname()
    Path("foo/bar")
    >>> Path('foo').dirname()
    Path(".")
    """
    if len(self.__path) == 1:
      return Path.dot
    else:
      return Path(self.__path[0:-1],
                  absolute = self.__absolute,
                  virtual = self.__virtual,
                  volume = self.__volume)

  def touch(self):
      """Create the designed file if it does not exists.

      Creates the parent directories if needed first.

      >>> Path('/tmp/.drake').remove()
      >>> p = Path('/tmp/.drake/.sub/.foo')
      >>> p.touch()
      >>> p.exists()
      True

      If the file does exist, this is a no-op.

      >>> path = Path('/tmp/.drake.touch.exists')
      >>> with open(str(path), 'w') as f:
      ...   print('foobar', file = f)
      >>> path.touch()
      >>> with open(str(path), 'r') as f:
      ...   print(f.read(), end = '')
      foobar
      """
      parent = self.dirname()
      if parent is not Path.dot:
        parent.mkpath()
      if not _OS.path.exists(str(self)):
        with open(str(self), 'w') as f:
          pass

  def mkpath(self):
      """Create the designed directory.

      Creates the parent directories if needed first.

      >>> Path('/tmp/.drake').remove()
      >>> p = Path('/tmp/.drake/.sub/')
      >>> p.mkpath()
      >>> p.exists()
      True
      """
      if not _OS.path.exists(str(self)):
          _OS.makedirs(str(self))

  def __eq__(self, rhs):
      """Whether self equals rhs.

      Pathes are equals if they have the same components and
      absoluteness.

      >>> Path('foo/bar') == Path('foo/bar')
      True
      >>> Path('foo/bar') == Path('foo')
      False
      >>> Path('foo/bar') == Path('bar/foo')
      False
      >>> Path('foo/bar') == Path('/foo/bar')
      False
      >>> Path('/foo/bar') == Path('/foo/bar')
      True
      """
      return self is drake.Path(rhs)

  def __truediv__(self, rhs):
    """The concatenation of self and rhs.

    rhs -- the end of the new path, as a Path or a string.

    >>> Path('foo/bar') / Path('bar/baz')
    Path("foo/bar/bar/baz")
    >>> Path('foo/bar') / 'baz'
    Path("foo/bar/baz")
    >>> Path('.') / 'baz'
    Path("baz")

    Contatenating an absolute path yields the path itself.

    >>> Path('foo') / Path('/absolute')
    Path("/absolute")
    """
    rhs = Path(rhs)
    if self is Path.dot:
      return rhs
    if rhs is Path.dot:
      return self
    if rhs.__absolute:
      return rhs
    return drake.Path(self.__path + rhs.__path,
                      absolute = self.__absolute,
                      virtual = self.__virtual,
                      volume = self.__volume)

  def prefix_of(self, rhs):
      """Whether self is a prefix of rhs.

      >>> p = Path('foo/bar')
      >>> p.prefix_of('foo/bar/baz/quux')
      True
      >>> p.prefix_of('foo/baz/bar/quux')
      False
      >>> p.prefix_of('nope')
      False
      """
      rhs = drake.Path(rhs).canonize().__path
      path = self.__path
      while len(rhs) and len(path) and path[0] == rhs[0]:
        rhs = rhs[1:]
        path = path[1:]
      return len(path) == 0

  def without_prefix(self, rhs, force = True):
      """Remove rhs prefix from self.

      rhs -- the prefix to strip, as a Path or a string.

      >>> p = Path('foo/bar/baz/quux')
      >>> p
      Path("foo/bar/baz/quux")
      >>> p.without_prefix("foo/bar")
      Path("baz/quux")
      >>> p = Path('/foo/bar/baz')
      >>> p.absolute()
      True
      >>> p = p.without_prefix('/foo')
      >>> p
      Path("bar/baz")
      >>> p.absolute()
      False

      Rewinds if rhs is not a prefix of self.

      >>> p.without_prefix("quux")
      Path("../bar/baz")

      Rewinding a path entirerly yields the current directory.

      >>> Path('foo/bar').without_prefix('foo/bar')
      Path(".")
      """
      rhs = drake.Path(rhs).canonize().__path
      path = self.__path
      while len(rhs) and len(path) and path[0] == rhs[0]:
        rhs = rhs[1:]
        path = path[1:]
      if not force and len(rhs) > 0:
        return self
      # FIXME: naive if rhs contains some '..'
      assert '..' not in rhs
      path = ('..',) * len(rhs) + path
      if not path:
        path = ('.',)
      return drake.Path(path,
                        absolute = False,
                        virtual = False,
                        volume = '')

  def __len__(self):
      return len(self.__path)

  def without_suffix(self, rhs):
      """Remove rhs suffix from self.

      rhs -- the suffix to strip, as a Path or a string.

      >>> p = Path('foo/bar/baz/quux')
      >>> p
      Path("foo/bar/baz/quux")
      >>> p.without_suffix("baz/quux")
      Path("foo/bar")

      Throws if rhs is not a prefix of self.

      >>> p.without_suffix("baz")
      Traceback (most recent call last):
          ...
      Exception: baz is not a suffix of foo/bar/baz/quux
      """
      rhs = drake.Path(rhs)
      if self.__path[-len(rhs.__path):] != rhs.__path:
        raise Exception("%s is not a suffix of %s" % (rhs, self))
      path = self.__path[0:-len(rhs.__path):]
      if not path:
        path = ('.',)
      return drake.Path(path,
                        absolute = self.__absolute,
                        virtual = self.__virtual,
                        volume = self.__volume)

  @classmethod
  def cwd(self):
      return Path(_OS.getcwd())

  def list(self):
      return _OS.listdir(str(self))

  class Pickler(pickle.Pickler):
    def persistent_id(self, obj):
      if isinstance(obj, drake.Path):
        return (
          obj._Path__path,
          obj.absolute(),
          obj.virtual,
          obj.volume,
        )
      else:
        return None

  class Unpickler(pickle.Unpickler):
    def persistent_load(self, obj):
      return drake.Path(
        obj[0],
        absolute = obj[1],
        virtual = obj[2],
        volume = obj[3],
      )

  def __iter__(self):
    return self.__path.__iter__()

  def __getnewargs__(self, *args, **kwargs):
    return (
      self.__path,
      self.__absolute,
      self.__virtual,
      self.__volume,
      str(self),
    )

  def __getstate__(self):
    pass

  def __setstate__(self):
    pass

Path.dot = Path('.')
Path.dotdot = Path('..')

_DEPFILE_BUILDER = Path('drake.Builder')

class DepFile:

    """File to store dependencies of a builder and their hash.

    To determine whether a builder should be executed, Drake has to
    check whether any of its sources has changed since the last
    build. This is done by comparing the hash of the current file with
    the hash of the file when the builder was last
    executed. Dependencies files store those hashes between
    consecutive runs.

    A dependency file is attached to a builder, and has a name since
    one builder may have several dependencies files if dependencies
    come from different sources. Each file stores several (file, hash)
    assocations.
    """

    def __init__(self, builder, name):
      """Construct a dependency file for builder with given name."""
      self.__builder = builder
      self.name = name
      self.__files = []
      self.__invalid = False
      self.__hashes = None
      self.__dirty = False

    @property
    def hashes(self):
      """The file hashes loaded from the disk."""
      return self.__hashes

    @property
    def dirty(self):
      '''Whether previous build failed.'''
      return self.__dirty

    @dirty.setter
    def dirty(self, dirty):
      self.__dirty = dirty
      self.save()

    def register(self, node, source = True):
        """Add the node to the hashed files."""
        self.__files.append((node, source))

    def path(self):
        """Path to the file storing the hashes."""
        return self.__builder.cachedir / self.name

    def read(self):
      """Read the hashes from the store file."""
      res = []
      if self.path().exists():
        with profile_unpickling():
          try:
            with open(str(self.path()), 'rb') as f:
              unpickled = drake.Path.Unpickler(f).load()
              if unpickled is None:
                self.__invalid = True
              if isinstance(unpickled, dict):
                self.__hashes = unpickled
              elif isinstance(unpickled, tuple):
                content = unpickled[1]
                self.__dirty = content['dirty']
                self.__hashes = content['hashes']
              else:
                self.__invalid = True
          except Exception:
            self.__invalid = True
      else:
        self.__hashes = {}

    Hashed = object()
    def up_to_date(self,
                   oldest_target,
                   oldest_mtime,
                   mtime_implemented):
      '''Whether targets are up to date wrt sources.

      False if building is needed. The most recent source mtime as
      a float if more recent than oldest_mtime. True otherwise.
      '''
      if self.__invalid:
        return False
      res = True
      for path, (old_hash, data) in self.__hashes.items():
        if old_hash is None:
          continue
        # FIXME: needed ?
        # if path not in Drake.current.nodes:
        #   del self.__hashes[path]
        #   continue
        n = node(path)
        if n.missing():
          explain(self.__builder, '%s disappeared' % path)
          return False
        if isinstance(n, Node):
          try:
            mtime = n.mtime
          except NotImplementedError:
            pass
          else:
            if mtime_implemented and mtime < oldest_mtime:
              continue
            elif res is True or res < mtime:
              res = mtime
            sched.logger.log(
              'drake.Builder.mtime',
              drake.log.LogLevel.debug,
              '%s: %s is more recent than %s (%s > %s)' % (
                self.__builder, n.path(), oldest_target,
                mtime, oldest_mtime))
        try:
          h = n.hash()
        except Exception:
          explain(self.__builder, '%s cannot be hashed' % path)
          return False
        if h != old_hash:
          explain(self.__builder, '%s has changed' % path)
          return False
      return res

    def update(self):
      """Rehash all files and write to the store file."""
      self.__hashes = dict(
        (node.name_absolute(), (node.hash() if source else None,
                                node.drake_type()))
        for node, source in self.__files)
      self.__dirty = False
      self.save()

    def save(self):
      content = {'hashes': self.__hashes, 'dirty': self.__dirty}
      with profile_pickling():
        path = self.path()
        with open(str(path), 'wb') as f:
          pickle.Pickler(f).dump((0, content))

    def remove(self):
      """Rehash all files and write to the store file."""
      self.path().remove()

    def __repr__(self):
        """Python representation."""
        return 'DepFile(%r, %r)' % (self.name, self.__builder)

    def __str__(self):
        """String representation."""
        return repr(self)

def path_build(path = None, absolute = False):
  """Return path as found in the build directory.

  This function prepend the necessary prefix to a path relative to
  the current drakefile to make it relative to the root of the build
  directory.

  When computing pathes in a drakefile, one might need to find the
  location of a path in the build directory relatively to this
  drakefile. Since the build is runned at the root of the build
  directory, if this drakefile is included from an other, path
  relative to this drakefile won't be valid.

  Most pathes are handled by nodes, so this function is rarely
  used. However it is sometimes necessary, when generating shell
  commands for instance.
  """
  if path is not None:
    path = Path(path)
    if path.absolute():
      return path
    else:
      path = drake.Drake.current.prefix / path
  else:
    path = drake.Drake.current.prefix
  if absolute:
    path = drake.Path(_OS.getcwd()) / path
  return path

def path_root():
    """The directory containing the root drakefile."""
    return Path(_OS.getcwd())

class _BaseNodeTypeType(type):

    node_types = {}

    def __call__(c, name, *arg, **kwargs):

        res = type.__call__(c, name, *arg, **kwargs)
        k = '%s.%s' % (res.__module__, res.__name__)
        _BaseNodeTypeType.node_types[k] = res
        return res

        return type.__call__(*arg)

class _BaseNodeType(type, metaclass = _BaseNodeTypeType):

  def __call__(c, *args, **kwargs):
    try:
      return type.__call__(c, *args, **kwargs)
    except NodeRedefinition as e:
      assert e.name() in Drake.current.nodes
      node = Drake.current.nodes[e.name()]
      if node.__class__ is not c:
        fmt = (node.__class__.__name__, node, c.__name__)
        raise Exception('%s %s redefined as a %s' % fmt)
      return node


class BaseNode(object, metaclass = _BaseNodeType):

  """Base entity manipulated by drake.

  Nodes represent the base elements that can be built or used as
  input by the drake buildsystem. Builders have a list of sources
  nodes, which are the node they use as input when they are
  executed, and produce nodes as output.

  Nodes are often attached to file (an input file in the case of a
  source node, a generated file in the case of a target node), in
  which case its type is Node."""

  uid = 0
  extensions = {}

  def __init__(self, name):
    """Create a node with the given name."""
    self.__name = name = name.canonize()
    if next(iter(name)) == '..':
      raise Exception('%s is outside the build directory' % name)
    if Drake.current.nodes.setdefault(self.__name, self) is not self:
      raise NodeRedefinition(self.__name)
    self.uid = BaseNode.uid
    BaseNode.uid += 1
    self._builder = None
    self.consumers = []
    self.__dependencies = sched.OrderedSet()
    self.__hash = None
    self.__skippable = False

  def name(self):
    '''The node name.'''
    return self.__name

  @property
  def name_relative(self):
    """Node name, relative to the current drakefile."""
    if self.__name.absolute():
      return self.__name
    return self.__name.without_prefix(drake.Drake.current.prefix)

  def name_absolute(self):
      """Node name, relative to the root of the source directory."""
      return self.__name

  def dot(self, marks):
      """Print a dot representation of this nodes build graph."""
      if (self in marks):
          return True
      marks[self] = None
      print('  node_%s [label="%s"]' % (self.uid, self.__name))
      if self._builder is not None:
          if self._builder.dot(marks):
              print('  builder_%s -> node_%s' % (self._builder.uid,
                                                 self.uid))
      return True

  @classmethod
  def drake_type(self):
      """The qualified name of this type."""
      return '%s.%s' % (self.__module__, self.__name__)

  def __str__(self):
      """String representation."""
      return str(self.__name)

  def __repr__(self):
      """Python representation."""
      return '%s(%s)' % (self.__class__.drake_type(), self.name())

  def build(self):
    """Build this node.

    Take necessary action to ensure this node is up to date. That
    is, roughly, run this node runner.
    """
    import sys
    if not _scheduled():
      Coroutine(self.build, str(self), Drake.current.scheduler)
      Drake.current.scheduler.run()
    else:
      with sched.logger.log(
          'drake.Builder',
          drake.log.LogLevel.trace,
          '%s: build', self):
        self._build()
        with sched.Scope() as scope:
          for dep in self.dependencies:
            if not dep.skippable():
              scope.run(dep.build, str(dep))
        self.polish()

  def _build(self):
    if self._builder is not None:
      self._builder.run()


  @property
  def build_status(self):
    return self._builder.build_status

  def polish(self):
      """A hook called when a node has been built.

      Called when a node has been built, that is, when all its
      dependencies have been built and the builder run. Default
      implementation does nothing.

      >>> class MyNode (Node):
      ...   def polish(self):
      ...     print('Polishing.')
      >>> n = MyNode('/tmp/.drake.polish')
      >>> n.path().remove()
      >>> b = TouchBuilder(n)
      >>> n.build()
      Touch /tmp/.drake.polish
      Polishing.
      """
      pass

  def clean(self):
      """Clean recursively for this node sources."""
      if self._builder is not None:
          self._builder.clean()

  def missing(self):
      """Whether this node is missing and must be built.

      Always False, so unless redefined, BaseNode are built only if
      a dependency changed.
      """
      return False

  def makefile_name(self):
      if isinstance(self, Node):
          return str(self.path())
      else:
          res = Path(self.name())
          res.virtual = False
          return str(res)

  def makefile(self, marks = None):
    """Print a Makefile for this node."""
    from pipes import quote
    if self._builder is None:
      return
    if marks is None:
      marks = set()
    if str(self.name()) in marks:
      return
    else:
      marks.add(str(self.name()))
    print('%s: %s' % (self.makefile_name(),
                      ' '.join(map(lambda n: n.makefile_name(),
                                   self.dependencies))))
    cmd = self._builder.command
    if cmd is not None:
      if isinstance(self, Node):
        print('\t@mkdir -p %s' % self.path().dirname())
      if not isinstance(cmd, tuple):
        cmd = (cmd,)
      for c in cmd:
        print('\t%s' % ' '.join(
            map(lambda a: quote(str(a)).replace('$', '$$'), c)))
    print('')
    for dependency in self.dependencies:
      dependency.makefile(marks)

  def report_dependencies(self, dependencies):
      """Called when dependencies have been built.

      This hook is always called no matter whether the nodes
      were successfully built or not.
      """
      pass

  @property
  def builder(self):
    return self._builder

  @builder.setter
  def builder(self, builder):
    del Drake.current.nodes[self._BaseNode__name]
    self._builder = builder
    Drake.current.nodes[self._BaseNode__name] = self

  def dependency_add(self, dep):
    assert dep is not None
    self.__dependencies.add(dep)

  def dependencies_add(self, deps):
    for dep in deps:
      self.dependency_add(dep)

  @property
  def dependencies(self):
      return self.__dependencies

  @property
  def dependencies_recursive(self):
    for dep in self.__dependencies:
      yield dep
      for sub in dep.dependencies_recursive:
        yield sub

  def __lt__(self, rhs):
    """Arbitrary global order on nodes, to enable
    sorting/indexing."""
    return self.name_absolute() < rhs.name_absolute()

  def hash(self):
    """Digest of the file as a string."""

    def _hash_file(hasher, path):
      if _OS.path.isdir(str(path)):
        for sub_path in _OS.listdir(str(path)):
          _hash_file(hasher, _OS.path.join(str(path), str(sub_path)))
      else:
        with open(str(path), 'rb') as f:
          while True:
            chunk = f.read(8192)
            if not chunk:
              break
            hasher.update(chunk)

    if self.__hash is None:
      with profile_hashing():
        hasher = hashlib.sha1()
        hashable = (
          dep for dep in chain((self,), self.dependencies)
          if isinstance(dep, Node))
        for node in sorted(hashable):
          _hash_file(hasher, node.path())
        self.__hash = hasher.digest()
    return self.__hash

  def skippable(self):
    if self.__skippable:
      return True
    self.__skippable = self._skippable()
    return self.__skippable

  def _skippable(self):
    if self.builder is None:
      if self.missing():
        return False
    else:
      if not self.builder._Builder__executed:
        return False
      if self.builder._Builder__executed_exception is not None:
        raise self.builder._Builder__executed_exception
    return all(dep.skippable() for dep in self.dependencies)

  @property
  def mtime_local(self):
    raise NotImplementedError()


class VirtualNode(BaseNode):

    """BaseNode that does not represent a file.

    These may be configuration or meta information such as the version
    system revision, used by other nodes as sources. They are also
    used to implement Rule, which is a node that recursively builds
    other nodes, but does not directly produce a file.
    """

    def __init__(self, name):
        """Create a virtual node with the given name."""
        path = drake.Drake.current.prefix / name
        path = drake.Path(path._Path__path, False, True)
        BaseNode.__init__(self, path)


class Node(BaseNode):

  """BaseNode representing a file."""

  def __init__(self, path):
    """Construct a Node with the given path."""
    path = drake.Drake.current.prefix / path
    BaseNode.__init__(self, path)
    self.__exists = False
    self.__mtime = None
    self.__path = None
    self.__path_absolute = None

  def clone(self, path):
        """Clone of this node, with an other path."""
        return Node(path)

  def clean(self):
        """Clean this node's file if it is generated, and recursively
        its sources recursively."""
        BaseNode.clean(self)
        if self._builder is not None and self.path().exists():
            print('Deleting %s' % self)
            _OS.remove(str(self.path()))

  def path(self, absolute = False):
    """Filesystem path to node file, relative to the root of the
    build directory.

    >>> with Drake('source/tree') as drake:
    ...   n = node('file1')
    ...   print(n.path())
    ...   n = node('file2')
    ...   builder = TouchBuilder([n])
    ...   print(n.path())
    source/tree/file1
    file2
    """
    if self.__path is None:
      name = self._BaseNode__name
      if name.absolute() or name.virtual:
        self.__path = name
        self.__path_absolute = name
      elif self._builder is None:
        self.__path = drake.path_source() / name
      else:
        self.__path = name
    if absolute:
      if self.__path_absolute is None:
        self.__path_absolute = drake.path_root() / self.__path
      return self.__path_absolute
    else:
      return self.__path

  def missing(self):
    """Whether the associated file doesn't exist.

    Nodes are built if their file does not exist.
    """
    if not self.__exists:
      self.__exists = self.path().exists()
    return not self.__exists

  def build(self):
    """Builds this node.

    Building a Node raises an error if the associated file does
    not exist and it has no builder.

    >>> n = node('/tmp/.drake.node')
    >>> n.path().remove()
    >>> n.build()
    Traceback (most recent call last):
        ...
    drake.NoBuilder: no builder to make /tmp/.drake.node

    If the file exist, drake consider it is a provided input and
    building it does nothing.

    >>> n.path().touch()
    >>> n.build()

    If a Node needs to be built and its builder is executed, it
    must create the Node's associated file.

    >>> n = node('/tmp/.drake.othernode')
    >>> n.path().remove()
    >>> class EmptyBuilder(Builder):
    ...   def execute(self):
    ...     return True
    >>> builder = EmptyBuilder([], [n])
    >>> n.build()
    Traceback (most recent call last):
        ...
    Exception: /tmp/.drake.othernode wasn't created by EmptyBuilder
    """
    super().build()

  def _build(self):
    if self._builder is None:
      if self.missing():
        raise NoBuilder(self)
    else:
      self._builder.run()

  def __repr__(self):
        """Filesystem path to the node file, as a string."""
        return str(self.path())

  @property
  def install_command(self):
      return None

  @property
  def mtime(self):
    return max(chain(
      (self.mtime_local,),
      (d.mtime for d in self.dependencies if isinstance(d, Node))))

  @property
  def mtime_local(self):
    if self.__mtime is None:
      self.__mtime = _OS.path.getmtime(str(self.path()))
    return self.__mtime

  def touch(self, t):
    _OS.utime(str(self.path()), None)
    self.__mtime = None
    while self.mtime <= t:
      _OS.utime(str(self.path()), None)
      self.__mtime = None

def node(path, type = None):
  """Create or get a BaseNode.

  path -- path to the node file.
  type -- optional type of the node.

  The returned node is determined as follow:
  * If a node exists for the given path, it is returned.
  * If the type is given, a node of that type is constructed with
    the path as argument.
  * If the path as a known extension, a node of the associated type
    is constructed with the path as argument.
  * A simple Node with that path is constructed.
  """
  if path.__class__ is not Path:
    path = Path(path)
  d = Drake.current
  res = d.nodes.get(drake.path_build() / path, None)
  if res is None:
    if type is not None:
      res = type(path)
    else:
      extension = path.extension
      extension_type = Node.extensions.get(extension, None)
      if extension_type is not None:
        res = extension_type(path)
      else:
        last_extension = extension.split('.')[-1]
        last_extension_type = Node.extensions.get(last_extension, None)
        if last_extension_type is not None:
          res = last_extension_type(path)
        else:
          res = Node(path)
  return res


def nodes(*paths, type = None):
    """Call node() on each given path and return the list of results.

    nodes('foo', 'bar', ...) is equivalent to
    [node('foo'), node('bar'), ...]
    """
    return list(map(lambda p: node(p, type = type), paths))


def command(cmd, cwd = None, stdout = None, env = None):
  """Run the shell command.

  cmd -- the shell command.
  cwd -- the dir to chdir to before.
  """
  if cwd is not None:
    cwd = str(cwd)
  try:
    if env:
      env = {k: str(v) for k, v in env.items()}
    p = subprocess.Popen(cmd, cwd = cwd, stdout = stdout, env = env)
    p.wait()
    return p.returncode == 0
  except getattr(__builtins__,'FileNotFoundError', IOError) as e:
    print(e, file = sys.stderr)
    return False


def command_flatten(command, env = None):
  if env is not None:
    output_env = ('%s=%s' % (var, pipes.quote(str(value)))
                  for var, value in env.items())
  else:
    output_env = ()
  return ' '.join(chain(output_env, map(pipes.quote, command)))

class Builder:

  """Produces a set of BaseNodes from an other set of BaseNodes."""

  builders = []
  uid = 0

  name = 'build'
  _deps_handlers = {}

  class Failed(Exception):

    def __init__(self, builder):
      self.__builder = builder

    def __str__(self):
      return '%s failed' % self.__builder

    @property
    def builder(self):
      return self.__builder

  @classmethod
  def register_deps_handler(self, name, f):
    """Add a dependency handler."""
    self._deps_handlers[name] = f

  def __init__(self, srcs, dsts, create_directories = True):
    """Create a builder.

    srcs -- List of source nodes, or source node if there is
            only one.
    dsts -- List of target nodes, or target node if there is
            only one.
    """
    self.__create_dirs = create_directories
    self.__sources = {}
    self.__sources_dyn = {}
    for src in srcs:
      self.add_src(src)
    self.__targets = []
    for dst in dsts:
      if dst.builder is not None:
        raise BuilderRedefinition(dst, dst.builder, self)
      self.__targets.append(dst)
      dst.builder = self
    self.uid = Builder.uid
    Builder.uid += 1
    Builder.builders.append(self)
    self._depfiles = {}
    self._depfile = DepFile(self, 'drake')
    self.__depfile_builder = DepFile(self, 'drake.Builder')
    self.__executed = False
    self.__executed_exception = None
    self.__executed_signal = None

  def sources_dynamic(self):
    """The list of dynamic source nodes."""
    return self.__sources_dyn.values()

  def sources(self):
    """The list of source nodes."""
    return self.__sources

  def targets(self):
    """The list of target nodes."""
    return self.__targets

  @property
  def path_stdout(self):
    return self.cachedir / 'stdout'

  @property
  def path_tmp(self):
    res = self.cachedir / 'tmp'
    res.mkpath()
    return res

  def cmd(self,
          pretty,
          cmd,
          cwd = None,
          leave_stdout = False,
          env = None,
          throw = False,
          redirect_stdout = None):
    """Run a shell command.

    pretty  -- A pretty version for output.
    command -- The command.

    The expansion handles shell escaping. The pretty version is
    printed, except if drake is in raw mode, in which case the
    actual command is printed.
    """
    if all([leave_stdout, redirect_stdout]):
      raise Exception(
        'specify either leave_stdout or redirect_stdout')
    if leave_stdout:
      out_file = None
    elif redirect_stdout is not None:
      if isinstance(redirect_stdout, Node):
        out_file = str(redirect_stdout.path())
      else:
        out_file = str(redirect_stdout)
    else:
      out_file = str(self.path_stdout)
    if not isinstance(cmd, tuple):
      cmd = (cmd,)
    def fun():
      if not _RAW and pretty is not None:
        self.output(pretty)
      for c in cmd:
        def convert(e):
          if isinstance(e, Node):
            return str(e.path())
          else:
            return str(e)
        c = list(map(convert, c))
        if _RAW or pretty is None:
          self.output(command_flatten(c, env))
        with contextlib.ExitStack() as stack:
          if out_file:
            stdout = stack.enter_context(open(out_file, 'w'))
          else:
            stdout = None
          if not command(c, cwd = cwd, stdout = stdout, env = env):
            if throw:
              raise Exception(
                'command failed: %s' % command_flatten(c, env))
            else:
              return False
      return True
    return self._run_job(fun)

  def output(self, raw, pretty = None):
    """Output pretty, or raw if drake is in raw mode."""
    if not _SILENT:
      print((not _RAW and pretty) or raw)


  CACHEDIR = Path('.drake')

  @property_memoize
  def cachedir(self):
    """The cachedir that stores dependency files."""
    path = self.__targets[0].name()
    if path.virtual:
      path = drake.Path(path._Path__path, False, False)
    if path.absolute():
      rel = drake.Path(path._Path__path, False, False)
      return Builder.CACHEDIR / rel
    else:
      rel = drake.Path(drake.path_build(absolute = True)._Path__path,
                       False, False)
      return Builder.CACHEDIR / rel / path

  def hash(self):
    """A hash for this builder"""
    return None

  def dependencies(self):
    """Recompute dynamic dependencies list and return them.

    Reimplemented by subclasses. This implementation returns an
    empty list.
    """
    pass

  def depfile(self, name):
    """The depfile for this node for static dependencies."""
    if name not in self._depfiles:
      self._depfiles[name] = DepFile(self, name)
    return self._depfiles[name]

  def add_dynsrc(self, name, node, data = None, source = True):
    """Add a dynamic source node."""
    self.depfile(name).register(node, source = source)
    if source:
      self.__sources_dyn[node.path()] = node

  def get_type(self, tname):
    """Return the node type with the given name."""
    return _BaseNodeTypeType.node_types[tname]

  def report_dependencies(self, dependencies):
    """Called when dependencies have been built.

    This hook is always called no matter whether the nodes
    were successfully built or not.
    """
    pass

  def __report_dependencies(self, dependencies):
    self.report_dependencies(dependencies)
    for target in self.__targets:
      target.report_dependencies(dependencies)

  @property
  def build_status(self):
    if not self.__executed:
      return None
    else:
      return self.__executed_exception is None

  def run(self):
    """Build sources recursively, check if our target are up to
    date, and executed if needed."""
    with sched.logger.log(
        'drake.Builder',
        drake.log.LogLevel.trace,
        '%s: run', self):
      if not self.__executed:
        # If someone is already executing this builder, wait.
        if self.__executed_signal is not None:
          sched.logger.log('drake.Builder',
                           drake.log.LogLevel.trace,
                           '%s: already being built, waiting', self)
          sched.wait(self.__executed_signal)
          sched.logger.log('drake.Builder',
                           drake.log.LogLevel.trace,
                           '%s: was built, resuming', self)
        # Otherwise, build it ourselves
        else:
          self.__executed_signal = sched.Signal()
      # If we were already executed, just skip
      if self.__executed:
        if self.__executed_exception is not None:
          sched.logger.log(
            'drake.Builder',
            drake.log.LogLevel.trace,
            '%s: already built in this run with an exception', self)
          raise self.__executed_exception
        sched.logger.log('drake.Builder',
                         drake.log.LogLevel.trace,
                         '%s: already built in this run', self)
        return
      try:
        # The list of static dependencies is now fixed
        for source in self.__sources.values():
          self._depfile.register(source)
        # Reload dynamic dependencies
        execute = self.__reload_dyndeps()
        coroutines_static = []
        coroutines_dynamic = []
        # FIXME: symetric of can_skip_node: if a node is a
        # plain file and does not exist, err immediately (or
        # execute = True).
        # Build static dependencies
        with logger.log('drake.Builder',
                        drake.log.LogLevel.debug,
                        '%s: build static dependencies', self):
          run_builders = set()
          with sched.Scope() as scope:
            for node in self.__sources.values():
              if node.skippable():
                continue
              if node.builder in run_builders and not node.dependencies:
                continue
              scope.run(node.build, str(node))
        # Build dynamic dependencies
        with logger.log('drake.Builder',
                        drake.log.LogLevel.debug,
                        '%s: build dynamic dependencies', self):
          run_builders = set()
          try:
            with sched.Scope(exception_join = True) as scope:
              for node in self.__sources_dyn.values():
                if node.skippable():
                  continue
                if node.builder in run_builders and not node.dependencies:
                  continue
                scope.run(node.build, str(node))
          except Exception as e:
            logger.log(
              'drake.Builder',
              drake.log.LogLevel.trace,
              '%s: error building dynamic dependency: %s', self, e)
            explain(
              self,
              'some dynamic dependency couldn\'t be built: %s' % e)
            execute = True
        # If any non-virtual target is missing, we must rebuild.
        mtime_implemented = True
        oldest_target = None
        oldest_mtime = None
        if not execute:
          for dst in self.__targets:
            if dst.missing():
              explain(self, 'target %s is missing' % dst)
              execute = True
              break
            else:
              try:
                mtime = dst.mtime_local
                if oldest_target is None or mtime < oldest_mtime:
                  oldest_target = dst
                  oldest_mtime = mtime
              except NotImplementedError:
                mtime_implemented = False
        # Load static dependencies
        self._depfile.read()
        if self._depfile._DepFile__invalid:
          explain(self,
                  'dependency file %s is invalid' % self._depfile)
          execute = True
        if self._depfile.dirty:
          explain(self, 'previous build failed')
          execute = True
        # If a new dependency appeared, we must rebuild.
        if not execute:
          for source in self.__sources.values():
            path = source.name_absolute()
            if path not in self._depfile.hashes:
              explain(self, 'of new dependency %s' % path)
              execute = True
              break
        # Check if we are up to date wrt to the builder itself
        self._builder_hash = self.hash()
        depfile_builder = self.cachedir / _DEPFILE_BUILDER
        if not execute:
          if self._builder_hash is not None:
            if depfile_builder.exists():
              try:
                with open(str(depfile_builder), 'rb') as f:
                  stored_hash = pickle.Unpickler(f).load()
              except Exception:
                explain(self, 'the builder hash is invalid')
                execute = True
              if not execute and self._builder_hash != stored_hash:
                explain(self, 'hash for the builder is outdated')
                execute = True
            else:
              explain(self, 'the builder hash is missing')
              execute = True
        # Check if we are up to date wrt all dependencies
        if not execute:
          for f in chain((self._depfile,), self._depfiles.values()):
            res = f.up_to_date(
              oldest_target, oldest_mtime, mtime_implemented)
            if not res:
              execute = True
          if mtime_implemented and not execute \
             and isinstance(res, float):
            for dst in self.__targets:
              if dst.mtime_local < oldest_mtime:
                print('Adjust mtime of %s' % dst)
                dst.touch(res)
        if execute:
          self._execute(depfile_builder)
        else:
            self.__executed = True
            logger.log('drake.Builder',
                       drake.log.LogLevel.debug,
                       '%s: everything is up to date', self)
      except Exception as e:
        logger.log('drake.Builder',
                   drake.log.LogLevel.trace,
                   '%s: exception: %s', self, e)
        self.__executed_exception = e
        raise
      else:
        logger.log('drake.Builder',
                   drake.log.LogLevel.debug,
                   '%s: done', self)
      finally:
        self.__executed = True
        self.__executed_signal.signal()

  def _execute(self, depfile_builder):
    with contextlib.ExitStack() as ctx:
      if not Drake.current.kill_builders_on_failure:
        ctx.enter_context(sched.NonInterruptible())
      self.cachedir.mkpath()
      if self.__create_dirs:
        for target in self.__targets:
          if isinstance(target, Node):
            target.path().dirname().mkpath()
      with logger.log('drake.Builder',
                      drake.log.LogLevel.trace,
                      '%s: needs execution', self):
        # Regenerate dynamic dependencies
        self.__sources_dyn = {}
        self._depfiles = {}
        with logger.log(
            'drake.Builder',
            drake.log.LogLevel.debug,
            '%s: recompute dynamic dependencies', self):
          self.dependencies()
        with logger.log(
            'drake.Builder',
            drake.log.LogLevel.debug,
            '%s: build dynamic dependencies', self):
          for node in self.__sources_dyn.values():
            # FIXME: parallelize
            node.build()
        self._builder_hash = self.hash()
        try:
          with logger.log('drake.Builder',
                          drake.log.LogLevel.trace,
                          '%s: execute', self):
            self._depfile.dirty = True
            success = self.execute()
            for dst in self.__targets:
              dst._Node__mtime = None
            logger.log('drake.Builder',
                       drake.log.LogLevel.trace,
                       '%s: executed', self)
        except sched.Terminate:
          raise
        except Exception as e:
          e_pretty = str(e)
          if not e_pretty:
            e_pretty = repr(e)
          print('%s: %s' % (self, e_pretty), file = sys.stderr)
          if 'DRAKE_DEBUG_BACKTRACE' in _OS.environ:
            import traceback
            traceback.print_exc()
          raise Builder.Failed(self) from e
        if not success:
          raise Builder.Failed(self)
        # Check every non-virtual target was built.
        with logger.log(
            'drake.Builder',
            drake.log.LogLevel.trace,
            '%s: check all targets were built', self):
          for dst in self.__targets:
            if dst.missing():
              raise Exception('%s wasn\'t created by %s' % (dst, self))
            if isinstance(dst, Node):
              dst._Node__hash = None
        # Update depfiles
        with logger.log('drake.Builder',
                        drake.log.LogLevel.debug,
                        '%s: write dependencies file %s',
                        self, self._depfile):
          self._depfile.update()
        if self._builder_hash is None:
          logger.log('drake.Builder',
                     drake.log.LogLevel.debug,
                     '%s: remove builder dependency file %s',
                     self, depfile_builder)
          depfile_builder.remove()
        else:
          logger.log('drake.Builder',
                     drake.log.LogLevel.debug,
                     '%s: write builder dependency file %s',
                     self, depfile_builder)
          with open(str(depfile_builder), 'wb') as f:
            pickle.Pickler(f).dump(self._builder_hash)
        # FIXME: BUG: remove dynamic dependencies files
        # that are no longer present, otherwise this will
        # be rebuilt forever.
        for f in self._depfiles.values():
          logger.log('drake.Builder',
                     drake.log.LogLevel.debug,
                     '%s: write dependencies file %s',
                     self, f)
          f.update()
        self.__executed = True

  def __reload_dyndeps(self):
    cachedir = self.cachedir
    if _OS.path.exists(str(cachedir)):
      for f in _OS.listdir(str(cachedir)):
        if not _OS.path.isfile(str(cachedir / f)):
          continue
        if f in ['drake', 'drake.Builder', 'stdout']:
          continue
        depfile = self.depfile(f)
        depfile.read()
        if depfile._DepFile__invalid:
          explain(self, 'dependency file %s is invalid' % f)
          return True
        handler = self._deps_handlers[f]
        with sched.logger.log(
            'drake.Builder',
            drake.log.LogLevel.dump,
            '%s: consider dependencies file %s', self, f):
          for path, (hash, data) in depfile.hashes.items():
            if path not in self.__sources and path not in self.__sources_dyn:
              node = handler(self, path, self.get_type(data), None)
              if node is not None:
                sched.logger.log(
                  'drake.Builder',
                  drake.log.LogLevel.dump,
                  '%s: add %s to sources', self, path)
                self.__sources_dyn[node.path()] = node

  def execute(self):
    """Generate target nodes from source node.

    Must be reimplemented by subclasses.
    """
    raise Exception('execute is not implemented for %s' % self)

  def clean(self):
    """Clean source nodes recursively."""
    for node in self.__sources.values():
      node.clean()

  def __str__(self):
    """String representation."""
    return self.__class__.__name__

  def add_src(self, src):
    """Add a static source."""
    self.__sources[src._BaseNode__name] = src
    src.consumers.append(self)

  def all_srcs(self):
    """All sources, recursively."""
    res = []
    for src in self.__sources.values() + self.__sources_dyn.values():
        res.append(src)
        if src.builder is not None:
            res += src.builder.all_srcs()
    return res

  def dot(self, marks):
    """Print a dot representation of the build graph."""
    if (self in marks):
        return True
    marks[self] = None

    print('  builder_%s [label="%s", shape=rect]' % \
          (self.uid, self.__class__))
    for node in itertools.chain(self.__sources.values(),
                                self.__sources_dyn.values()):
        if node.dot(marks):
            print('  node_%s -> builder_%s' % (node.uid, self.uid))
    return True

  def _run_job(self, job):
    if Drake.current.jobs_lock is not None:
      with Drake.current.jobs_lock:
        return sched.background(job)
    else:
      return job()

  def cleanup_source_directory(self, root_path):
    root_path = drake.Path(root_path)
    effective = set()
    for path, dirs, files in _OS.walk(str(root_path)):
      path = drake.Path(path)
      for f in chain(files, dirs):
        effective.add(path / f)
    expected = set(
      chain(*[n.path().unfold()
              for s in self.sources().values()
              for n in chain([s], s.dependencies_recursive)]))
    for g in drake.Path.rootify(p.without_prefix(root_path)
                                for p in (effective - expected)):
      g = root_path / g
      self.output('Cleanup %s' % g)
      g = str(g)
      if _OS.path.isdir(g):
        shutil.rmtree(str(g))
      else:
        _OS.remove(g)


class ShellCommand(Builder):

    """A builder that runs a shell command.

    This builder is a commodity to create a builder that simply runs a
    shell commands, or to subclass so you don't need to reimplement
    execute.

    >>> path = Path("/tmp/.drake.foo")
    >>> n = node("/tmp/.drake.foo")
    >>> b = ShellCommand([], [n], ['touch', '/tmp/.drake.foo'])
    >>> path.remove()
    >>> n.build()
    touch /tmp/.drake.foo
    >>> path.exists()
    True
    """

    def __init__(self, sources, targets, command,
                 pretty = None,
                 cwd = None,
                 environment = None,
                 stdout = None):
        """Create a builder that runs command.

        sources -- List of source nodes, or source source node if
                   there's only one.
        targets -- List of target nodes, or target target node if
                   there's only one.
        command -- The shell command to run.
        pretty  -- Optional pretty printing.
        """
        if isinstance(stdout, Node) and stdout not in targets:
          targets.append(stdout)
        Builder.__init__(self, sources, targets)
        self.__command = command
        self.__pretty = pretty
        self.__cwd = cwd
        self.__environment = environment
        self.__stdout = stdout


    def execute(self):
        """Run the command given at construction time."""
        return self.cmd(self.__pretty or ' '.join(self.__command),
                        self.__command,
                        cwd = self.__cwd,
                        env = self.__environment,
                        redirect_stdout = self.__stdout)

    @property
    def command(self):
        return self.__command

    def hash(self):
      return self.__command

    def __str__(self):
      return 'ShellCommand(%s)' % (self.__pretty or '')

class Dictionary(VirtualNode):

    """A virtual node that represents a dictionary.

    This kind of node is useful to represent a set of key/value
    association that can be used as in input source for a builder,
    such as configuration.
    """

    def __init__(self, name, content = {}):
        """Build a dictionary with given content.

        name    -- The node name.
        content -- The content, as a dictionary.
        """
        VirtualNode.__init__(self, '%s' % name)
        self.content = content

    def hash(self):
        """Hash value."""
        # FIXME: sha1 of the string repr ain't optimal
        items = list(self)
        items.sort()
        return hashlib.sha1(str(items).encode('utf-8')).hexdigest()

    def __iter__(self):
        """Iterate over the (key, value) pairs."""
        return iter(self.content.items())


class Converter(Builder):
  """A builder that convert a node to another one.

  This builder itself doesn't do anything. It is used as a flag to
  discover a node is convertible to another expected type.
  """
  def __init__(self, source, target, additional_sources = []):
    super().__init__(itertools.chain([source], additional_sources),
                     [target])
    self.__source = source
    self.__target = target

  @property
  def target(self):
    return self.__target

  @property
  def source(self):
    return self.__source


class Expander(Builder):

    """A builder that expands content of Dictionary in text.

    This class becomes useful when subclass define the content()
    method, that returns text in which to expand values. See
    FileExpander, TextExpander and FunctionExpander.

    >>> class MyExpander(Expander):
    ...     def __init__(self, content, *args, **kwargs):
    ...         Expander.__init__(self, *args, **kwargs)
    ...         self.__content = content
    ...     def content(self):
    ...         return self.__content
    >>> colors  = Dictionary('colors',  { 'apple-color':  'red',
    ...                                   'banana-color':  'yellow' })
    >>> lengths = Dictionary('lengths', { 'apple-length': 10,
    ...                                   'banana-length': 15 })
    >>> target = Node('/tmp/.drake.expander.1')
    >>> builder = MyExpander(
    ...   'Apples are @apple-color@, bananas are @banana-color@.',
    ...   [colors, lengths], target)
    >>> target.path().remove()
    >>> target.build()
    Expand /tmp/.drake.expander.1
    >>> with open('/tmp/.drake.expander.1') as f:
    ...   content = f.read()
    >>> content
    'Apples are red, bananas are yellow.\\n'

    The expanded pattern can me configured by setting a custom
    matcher. The matcher must be a regexp that contains at least one
    group, it is searched in the content, then the match of the first
    group is used as a key to search source dictionaries, and the
    whole match is replaced with the obtained value. For instance, the
    default '@([a-zA-Z0-9_-]+)@' will match autoconf-style variables,
    '@name@'. Here is an example with shell-style variables $name
    (except dashes are accepted):

    >>> target = Node('/tmp/.drake.expander.2')
    >>> builder = MyExpander('Bananas are $banana-length '
    ...                      'centimeters long.',
    ...                      [colors, lengths], target,
    ...                      matcher = '\\$([a-zA-Z0-9][-_a-zA-Z0-9]*)')
    >>> target.path().remove()
    >>> target.build()
    Expand /tmp/.drake.expander.2
    >>> with open('/tmp/.drake.expander.2') as f:
    ...   content = f.read()
    >>> content
    'Bananas are 15 centimeters long.\\n'

    The behavior in case a key is not found can be adjusted with
    missing_fatal:

    >>> target = Node('/tmp/.drake.expander.3')
    >>> builder = MyExpander('Kiwis are @kiwi-color@.',
    ...                      [colors, lengths], target)
    >>> print(builder.missing_fatal())
    True
    >>> target.path().remove()
    >>> target.build()
    Traceback (most recent call last):
      ...
    drake.Failed: MyExpander failed
    >>> target.builder = None
    >>> builder = MyExpander('Kiwis are @kiwi-color@.',
    ...                      [colors, lengths], target,
    ...                      missing_fatal = False)
    >>> builder.missing_fatal()
    False
    >>> target.build()
    Expand /tmp/.drake.expander.3
    >>> with open('/tmp/.drake.expander.3') as f:
    ...   content = f.read()
    >>> content
    'Kiwis are @kiwi-color@.\\n'
    """

    def __init__(self, dicts, target, sources = [],
               matcher = '@([a-zA-Z0-9_-]+)@', missing_fatal = True):
      """Create and expander that expands the given dictionaries.

      dicts         -- The dictionaries from which to expand keys.
      sources       -- List of additional source nodes,
                       or additional source node if there's only one.
      target        -- The target Node where to store the result.
      matcher       -- A regexp to find the patterns to expand in the
                       content.
      missing_fatal -- Whether a key in the content missing from the
                       dictionaries is fatal.
      """
      if not isinstance(dicts, list):
          dicts = [dicts]
      super().__init__(sources + dicts, [target])
      self.__dicts = dicts
      self.matcher = re.compile(matcher)
      self.__missing_fatal = missing_fatal
      self.__target = target

    def missing_fatal(self):
        return self.__missing_fatal

    def execute(self):
        """Expand the keys in the content and write to target file."""
        self.output('Expand %s' % (self.__target))
        vars = {}
        for d in self.__dicts:
            vars.update(dict(d))
        content = self.content()
        for match in self.matcher.finditer(content):
            key = match.group(1)
            try:
                content = content.replace(match.group(0),
                                          str(vars[key]))
            except KeyError:
                if self.__missing_fatal:
                    print('Missing expansion: %s' % key)
                    return False

        with open(str(self.__target.path()), 'w') as f:
            print(content, file = f)
        return True

    def dictionaries(self):
        """The list of source dictionary."""
        return self.__dicts

    def target(self):
        """The target Node."""
        return self.__target

class FileExpander(Expander):
    """An Expander that takes its content from a file.

    >>> source = Node('/tmp/.drake.file.expander.source')
    >>> with open(str(source.path()), 'w') as f:
    ...   print('Expand @this@.', file = f)
    >>> target = Node('/tmp/.drake.file.expander.target')
    >>> builder = FileExpander(source, [Dictionary('d_file',
    ...                                 { 'this': 'that' })], target)
    >>> target.path().remove()
    >>> target.build()
    Expand /tmp/.drake.file.expander.target
    >>> with open('/tmp/.drake.file.expander.target') as f:
    ...   content = f.read()
    >>> content
    'Expand that.\\n\\n'
    """
    def __init__(self, source, dicts, target = None, *args, **kwargs):
      """Create a file expander.

      source       -- The file to expand.
      args, kwargs -- Rest of the arguments for Expander constructor.
      """
      self.__source = source
      assert isinstance(source, BaseNode)
      if target is None:
        target = source.name().without_last_extension()
        target = node(target)
      else:
        assert isinstance(target, BaseNode)
        self.__target = target
      Expander.__init__(self,
                        dicts = dicts,
                        sources = [source],
                        target = target,
                        *args, **kwargs)

    def execute(self):
        if Expander.execute(self):
            shutil.copymode(str(self.__source.path()),
                            str(self.__target.path()))
            return True
        else:
            return False

    def content(self):
        """The content of the source file."""
        with open(str(self.__source.path()), 'r') as f:
          return f.read()

    def source(self):
        """The source node."""
        return self.__source


class TextExpander(Expander):
  """An Expander with a static content.

  >>> target = Node('/tmp/.drake.text.expander')
  >>> builder = TextExpander('Expand @this@.',
  ...                        [Dictionary('d_text',
  ...                                    { 'this': 'that' })], target)
  >>> target.path().remove()
  >>> target.build()
  Expand /tmp/.drake.text.expander
  >>> with open('/tmp/.drake.text.expander') as f:
  ...   content = f.read()
  >>> content
  'Expand that.\\n'
  """
  def __init__(self, text, *args, **kwargs):
      """Create a text expander.

      text         -- The text to expand.
      args, kwargs -- Rest of the arguments for Expander constructor.
      """
      self.__text = text
      Expander.__init__(self, *args, **kwargs)

  def content(self):
      """The text."""
      return self.__text;

  def text(self):
      """The text."""
      return self.__text

class FunctionExpander(Expander):

    """An Expander that maps a function on the dictionaries content.

    >>> target = Node('/tmp/.drake.function.expander')
    >>> import collections
    >>> dict = collections.OrderedDict()
    >>> dict['version_major'] = 4
    >>> dict['version_minor'] = 2
    >>> version = Dictionary('version', dict)
    >>> def define(k, v):
    ...     return '# define %s %s\\n' % (k.upper(), v)
    >>> builder = FunctionExpander(define, [version], target)
    >>> target.path().remove()
    >>> target.build()
    Expand /tmp/.drake.function.expander
    >>> with open('/tmp/.drake.function.expander') as f:
    ...   content = f.read()
    >>> content
    '# define VERSION_MAJOR 4\\n# define VERSION_MINOR 2\\n\\n'
    """

    def __init__(self, function, *args, **kwargs):
      """Create a function expander.=

      function     -- The function to apply on key, values pairs.
      args, kwargs -- Rest of the arguments for Expander constructor.
      """
      self.__function = function
      Expander.__init__(self, *args, **kwargs)

    def content(self):
        """The content obtained by mapping the function on the
        dictionaries."""
        res = ''
        for d in self.dictionaries():
            for key, value in d:
                res += self.__function(key, value)
        return res

    def function(self):
        """The function."""
        return self.__function


class _Module:

  def __init__(self, globals):
    self.globals = globals

  def __getattr__(self, name):
    return self.globals[name]

  def __contains__(self, key):
    return key in self.globals


def include(path, *args, **kwargs):
    """Include a sub-drakefile.

    path         -- Path to the directory where the drakefile is
                    located.
    args, kwargs -- Arguments for the drakefile's configure.

    Load the drakefile found in the specified directory, merge its
    graph with ours and return an object that has all variables
    defined globally by the sub-drakefile as attributes.
    """
    path = Path(path)
    with Drake.current.recurse(path):
      drakefile = None
      names = ['drakefile', 'drakefile.py']
      for name in names:
        path = drake.path_source(name)
        if path.exists():
          drakefile = path
          break
      if drakefile is None:
          raise Exception('cannot find %s or %s in %s' % \
                          (', '.join(names[:-1]), names[-1], path))
      res = _raw_include(str(drakefile), *args, **kwargs)
      return res


def _raw_include(path, *args, **kwargs):
  g = {
    'drake': drake,
  }
  #execfile(path, g)
  with open(path) as f:
    exec(compile('__file__ = "%s"\n' % (path) + f.read(), path, 'exec'), g)
  res = _Module(g)
  if 'configure' in res:
    res.configure(*args, **kwargs)
  return res

def dot(node, *filters):

    # FIXME: coro!
    node.build()
    marks = {}
    print('digraph')
    print('{')
    node.dot(marks)
    print('}')

_MODES = {}

def command_add(name, action):
    """Register a new command available from the command line.

    name   -- The name of the command.
    action -- The function called by the command.

    Using --name node_list on the command line will call action with
    the node list as argument.
    """
    _MODES[name] = action


class CWDPrinter:

  def __init__(self, path = None):
    self.__path = path or _OS.getcwd()

  def __enter__(self):
    print('%s: Entering directory `%s\'' % (sys.argv[0], self.__path))

  def __exit__(self, *args):
    print('%s: Leaving directory `%s\'' % (sys.argv[0], self.__path))


def _register_commands():


    def all_if_none(nodes):
        # Copy it, since it will change during iteration. This shouldn't
        # be a problem, all newly inserted will be dependencies of the
        # already existing nodes. Right?
        if len(nodes):
            return nodes
        else:
            return list(Drake.current.nodes.values())

    def build(nodes):
      with CWDPrinter():
        try:
          if not len(nodes):
            nodes = [node for node in Drake.current.nodes.values()
                     if not len(node.consumers)]
          coroutines = []
          for node in nodes:
            coroutines.append(Coroutine(node.build, str(node),
                                        Drake.current.scheduler))
          Drake.current.scheduler.run()
        except Builder.Failed as e:
          print('%s: *** %s' % (sys.argv[0], e))
          exit(1)
    command_add('build', build)

    def clean(nodes):
        with CWDPrinter():
            for node in all_if_none(nodes):
                node.clean()
    command_add('clean', clean)

    def dot_cmd(nodes):
        for node in all_if_none(nodes):
            dot(node)
    command_add('dot', dot_cmd)

    def dot_show_cmd(nodes):
        if not len(nodes):
            print('%s: dot-show: give me some nodes to show.' % \
                  sys.argv[0])
        for node in nodes:
            p = subprocess.Popen('dot -Tpng | xv -',
                                 shell = True,
                                 stdin = subprocess.PIPE)
            stdout = sys.stdout
            sys.stdout = p.stdin
            dot(node)
            p.communicate()
            sys.stdout = stdout
    command_add('dot-show', dot_show_cmd)

    def makefile(nodes):
        root_nodes = [node for node in Drake.current.nodes.values()
                      if not len(node.consumers)]
        if not len(nodes):
            nodes = root_nodes
        print('all: %s\n' % ' '.join(map(lambda n: n.makefile_name(),
                                         root_nodes)))
        marks = set()
        for node in nodes:
            node.makefile(marks)
    command_add('makefile', makefile)

_register_commands()

_ARG_DOC_RE = re.compile('\\s*(\\w+)\\s*--\\s*(.*)')
def _args_doc(doc):
    res = {}
    for line in doc.split('\n'):
        match = _ARG_DOC_RE.match(line)
        if match:
            res[match.group(1)] = match.group(2)
    return res


def help():
  print('%s [OPTIONS] [CONFIG] [ACTIONS]' % sys.argv[0])
  print('''\

OPTIONS:
\t--help, -h: print this usage and exit.
\t--jobs N, -j N: set number of concurrent jobs to N.
''')


  print('CONFIG:')
  doc = {}
  configure = Drake.current.configure
  if configure.__doc__ is not None:
    doc = _args_doc(configure.__doc__)
  specs = inspect.getfullargspec(configure)
  for arg in specs.args:
    type = str
    if arg in specs.annotations:
      type = specs.annotations[arg]
    if type is str:
      type = 'string'
    elif type is bool:
      type = 'boolean'
    sys.stdout.write('\t--%s=%s' % (arg, type))
    if arg in doc:
      print(': %s' % doc[arg])
    else:
      print()
  print('''\

ACTIONS:
	--build [NODES]: build NODES, or all nodes in NODES is empty.
	--clean [NODES]: recursively delete all generated ancestors of
	  NODES, or all generated nodes in NODES is empty.
	--dot NODES: generate a dot dependency graph on stdout for
	  NODES (requires dot).
	--dot-show NODES: show a dependency graph for NODES (requires
	  dot and xv).''')
  exit(0)

def complete_modes():
  for mode in sorted(_MODES.keys()):
    print('%s\tswitch to %s mode' % (mode, mode))
  exit(0)

def complete_nodes():
  def res():
    for node in Drake.current.nodes:
      print(node)
    exit(0)
  return res

def complete_options():
  print('-j,--jobs\tset the number of parallel jobs\tnumber of parallel jobs')
  print('-h,--help\tshow usage')
  exit(0)

_DEFAULTS = []

def add_default_node(node):
  _DEFAULTS.append(node)


def run(root, *cfg, **kwcfg):
  drake = Drake(root)
  drake.run(*cfg, **kwcfg)


class WritePermissions:

  def __init__(self, node):
    self.__path = str(node.path())
    self.__initial_permissions = None

  def __enter__(self):
    # FIXME: errors!
    try:
      self.__initial_permissions = _OS.stat(self.__path).st_mode
      _OS.chmod(self.__path,
                _OS.stat(self.__path).st_mode | stat.S_IWUSR)
    except OSError as e:
      self.__initial_permissions = None
      if e.errno == 2:
        pass
      else:
        raise

  @property
  def permissions(self):
    return self.__initial_permissions or \
      _OS.stat(self.__path).st_mode & ~stat.S_IWRITE

  def __exit__(self, *args):
    try:
      _OS.chmod(self.__path, self.permissions)
    except OSError as e:
      if e.errno == 2:
        pass
      else:
        raise


class Copy(Builder):

  """Builder to copy files.

  See the convenience function copy to copy multiple files easily.

  >>> source = node('/tmp/.drake.Copy.source')
  >>> with open(str(source.path()), 'w') as f:
  ...   print('Content.', file = f)
  >>> builder = Copy(source, Path('/tmp/.drake.Copy.dest'))
  >>> target = builder.target()
  >>> target
  /tmp/.drake.Copy.dest
  >>> target.path().remove()
  >>> builder.target().build()
  Copy /tmp/.drake.Copy.dest
  >>> with open(str(target.path()), 'r') as f:
  ...   content = f.read()
  >>> content
  'Content.\\n'
  """

  name = 'copy'

  @classmethod
  def __original(self, node):
    b = node.builder
    if b is not None and b.__class__ is Copy:
      return self.__original(b.source)
    else:
      return node

  def __init__(self, source, to, post_process = None):
    """Create a copy builder.

    source -- Node to copy.
    to     -- Destination path.
    """
    self.__source = source
    self.__target = source.clone(to.canonize())
    self.__post_process = post_process
    if self.__source is self.__target:
      return
    try:
      Builder.__init__(self, [self.__source], [self.__target])
    except BuilderRedefinition:
      if isinstance(self, Copy):
        previous = self.__target.builder
        if isinstance(previous, Copy):
          original = self.__original(previous.source)
          if original is self.__original(source):
            if self.__class__ is self.__target.builder.__class__:
              return
            else:
              drake.warn('%s is copied twice to %s '
                         'but with different builder types (%s and %s), '
                         'which is thus not considered equivalent' % (
                           original, self.__target,
                           self.__target.builder.name, self.name))
      raise

  @property
  def source(self):
    """The source node."""
    return self.__source

  def target(self):
    """The target node."""
    return self.__target

  def execute(self):
    """Copy the source to the target."""
    self.output('Copy %s to %s' % (self.__source.path(),
                                   self.__target.path()),
                'Copy %s' % self.__target)
    res = self._copy()
    if not res:
      return res
    if self.__post_process:
      res = self.__post_process(self.__target.path())
    return res

  def _copy(self):
    with WritePermissions(self.__target):
      try:
        shutil.copy2(str(self.__source.path()),
                     str(self.__target.path()))
      except PermissionError as e:
        # Landing here means that we didn't have permission to do a copystat as
        # part of the copy2. Fallback to a straight copy with a log.
        # This fixes an issue with OS X 10.11 not enough permissions for files
        # in /usr/lib.
        sched.logger.log(
          'drake.copy',
          drake.log.LogLevel.debug,
          'unable to copy2 %s, falling back to copy', str(self.__source.path()))
        shutil.copy(str(self.__source.path()),
                    str(self.__target.path()))

    return True

  @property
  def command(self):
    return ['cp', self.__source.path(), self.__target.path()]

  def __str__(self):
    return 'copy of %s' % self.target()


class Install(Copy):
  """Builder to install files.

  Same as copy, but also executes the node install hook.
  """

  name = 'install'

  def execute(self):
    self.output('Install %s to %s' % (self.source.path(),
                                   self.target().path()),
                'Install %s' % self.target())
    if not self._copy():
      return False
    if self.target().install_command is not None:
      with WritePermissions(self.target()):
        return self.cmd(pretty = None,
                        cmd = self.target().install_command)
    return True

  @property
  def command(self):
    cmd = super().command
    install_cmd = self.source.install_command
    if install_cmd is not None:
      return (cmd, install_cmd)
    else:
      return cmd

import collections
def __copy(sources, to, strip_prefix, builder, post_process):
  with sched.logger.log(
      'drake.copy',
      drake.log.LogLevel.trace,
      'copy %s to %s (strip: %s)', sources, to, strip_prefix):
    to = drake.Path(to)
    if isinstance(sources, types.GeneratorType):
      sources = list(sources)
    multiple = isinstance(sources, collections.Iterable)
    if multiple and not len(sources):
      return []
    if strip_prefix is not None:
      if strip_prefix is True:
        if multiple:
          strip_prefix = sources[0].name_relative.dirname()
        else:
          strip_prefix = sources.name_relative.dirname()
      else:
        strip_prefix = drake.Path(strip_prefix)
      if not strip_prefix.absolute():
        strip_prefix = drake.path_build(strip_prefix)
    else:
      strip_prefix = drake.path_build()
    if multiple:
      res = []
      for node in sources:
        res.append(__copy_stripped(node, to, strip_prefix, builder, post_process))
      return res
    else:
      return __copy_stripped(sources, to, strip_prefix, builder, post_process)

__copy_stripped_cache = {}
def __copy_stripped(source, to, strip_prefix, builder, post_process):
  key = (source, to, strip_prefix, builder)
  cache = __copy_stripped_cache.get(key)
  if cache is not None:
    return cache
  with sched.logger.log(
      'drake.copy',
      drake.log.LogLevel.debug,
      'stripped copy %s to %s (strip: %s)', source, to, strip_prefix):
    path = source.name_absolute()
    if strip_prefix is not None:
      path = path.without_prefix(strip_prefix)
    path = (to / path).canonize()
    sched.logger.log('drake.copy', drake.log.LogLevel.debug,
                     'copy as: %s', path)
    path_abs = drake.path_build(path)
    # Break install loops
    if path_abs in drake.Drake.current.nodes:
      res = drake.Drake.current.nodes[path_abs]
      if Copy._Copy__original(source) is res:
        __copy_stripped_cache[key] = res
        return res
    res = builder(source, path, post_process).target()
    for dep in source.dependencies:
      if not dep.name_absolute().absolute():
        node = __copy_stripped(dep, to, strip_prefix, builder, post_process)
        if node is not None:
          res.dependency_add(node)
    __copy_stripped_cache[key] = res
    return res

def copy(sources,
         to,
         strip_prefix = None,
         post_process = None,
         builder = Copy):
  """Convenience function to create Copy builders.

  When copying large file trees, iterating and creating Copy
  builders manually by computing the destination path can be a
  hassle. This convenience function provides a condensed mean to
  express common file trees copies, and returns the list of copied
  nodes.

  The sources nodes are copied in the to directory. The sources path
  is kept and concatenated to the destination directory. That is,
  copying 'foo/bar' into 'baz/quux' whill create the
  'baz/quux/foo/bar' node.

  If strip_prefix is specified, it is stripped from the source
  pathes before copying. That is, copying 'foo/bar/baz' into 'quux'
  with a strip prefix of 'foo' wil create the 'bar/baz/quux' node.

  sources      -- List of nodes to copy, or a single node to copy.
  to           -- Path where to copy.
  strip_prefix -- Prefix Path stripped from source pathes. If True,
                  only the filename is kept.

  >>> sources = [node('/tmp/.drake.copy.source/a'),
  ...            node('/tmp/.drake.copy.source/b')]
  >>> targets = copy(sources, '/tmp/.drake.copy.dest',
  ...                strip_prefix = '/tmp')
  >>> targets
  [/tmp/.drake.copy.dest/.drake.copy.source/a, /tmp/.drake.copy.dest/.drake.copy.source/b]
  """
  return __copy(sources, to, strip_prefix, builder, post_process)


def install(sources, to, strip_prefix = None, post_process = None):
  """Convenience function to create Install builders.

  See documentation of copy.
  """
  return __copy(sources, to, strip_prefix, Install, post_process)


class Rule(VirtualNode):

  """Virtual node that bounces to other nodes.

  Since rules are virtual nodes, creating an install rule as
  demonstrated below would enable to run `drake //install' to
  copy files.

  >>> sources = nodes('/tmp/.drake.rule1', '/tmp/.drake.rule2')
  >>> for source in sources:
  ...     source.path().touch()
  >>> targets = copy(sources, '/tmp/.drake.rule.dest',
  ...                strip_prefix = '/tmp')
  >>> for target in targets:
  ...     target.path().remove()
  >>> rule = Rule('install', targets)
  >>> rule.build()
  Copy /tmp/.drake.rule.dest/.drake.rule1
  Copy /tmp/.drake.rule.dest/.drake.rule2
  """

  def __init__(self, name, nodes = []):
    """Create a rule.

    name  -- Node name.
    nodes -- The node to build when the rule is built
    """
    VirtualNode.__init__(self, name)
    self << nodes

  def hash(self):
    """Hash value."""
    return ''

  def __lshift__(self, nodes):
    '''Add a node to build when the rule is built.

    >>> rule = Rule('name')
    >>> created = drake.touch('/tmp/.drake.rule.add')
    >>> created.path().remove()
    >>> rule << created
    >>> rule.build()
    Touch /tmp/.drake.rule.add
    '''
    if isinstance(nodes, (list, types.GeneratorType)):
      for node in nodes:
        self << node
    else:
      self.dependency_add(nodes)

class EmptyBuilder(Builder):

    """Builder which execution does nothing.

    Usefull to create dependencies between nodes.
    """

    def execute(self):
        """Do nothing."""
        return True

class WriteBuilder(Builder):
    """Builder that write a given input to files.

    >>> n = node('/tmp/.drake.write')
    >>> n.path().remove()
    >>> b = WriteBuilder('Hello world!', n)
    >>> n.build()
    Write /tmp/.drake.write
    >>> n.path().exists()
    True
    >>> with open(str(n.path()), 'r') as f:
    ...   print(f.read())
    Hello world!
    """

    def __init__(self, input, nodes):
        """Create a WriteBuilder.

        input -- Text or bytes.
        nodes -- target nodes list, or a single target node.
        """
        if not isinstance(input, bytes):
          input = bytes(input, encoding = 'utf-8')
        self.__input = input
        if isinstance(nodes, BaseNode):
            nodes = [nodes]
        for node in nodes:
            assert isinstance(node, Node)
        Builder.__init__(self, [], nodes)

    def execute(self):
        """Create all the non-existent target nodes as empty files."""
        pretty = 'Write' if len(self.__input) else 'Touch'
        self.output('%s %s' % (pretty, ', '.join(map(str, self.targets()))))
        for node in self.targets():
            assert isinstance(node, Node)
            node.path().touch()
            with WritePermissions(node):
              with open(str(node.path()), 'wb') as f:
                f.write(self.__input)
        return True

def write(body, path):
  res = node(Path(path))
  WriteBuilder(body, res)
  return res

class TouchBuilder(WriteBuilder):

    '''Builder that simply creates its targets as empty files.

    >>> n = node('/tmp/.drake.touchbuilder')
    >>> n.path().remove()
    >>> b = TouchBuilder(n)
    >>> n.build()
    Touch /tmp/.drake.touchbuilder
    >>> n.path().exists()
    True
    '''

    def __init__(self, nodes):
      super().__init__('', nodes)

    def __str__(self):
      return 'TouchBuilder(%r)' % self.targets()

def touch(path):
  res = node(Path(path))
  TouchBuilder(res)
  return res

# Architectures
class architecture(Enumerated,
                   values = ['x86', 'x86_64', 'arm']):
  pass

# OSes
class os:

    """Oses enum."""

    android = 0
    linux = 1
    macos = 2
    windows = 3
    ios = 4
    ios_simulator = 5


def reset():
    for node in Drake.current.nodes.values():
        if node.builder is not None:
            node.builder._Builder__built = False
            node.builder._Builder__sources_dyn = {}
    Drake.current._Drake__nodes = {}

# Configuration
class Configuration:

  def _search(self, what, where):
    return self._search_all(what, where)[0]

  def __split(self, pair):
    if isinstance(pair, tuple):
      return pair
    else:
      return pair, pair

  def _search_any(self, whats, wheres):
    for what in whats:
      res, path = self.__split(what)
      for where in wheres:
        if (where / path).exists():
          return where, res
    raise Exception(
      'unable to find %s in %s.' % \
        (self._format_search((self.__split(what)[1]
                              for what in whats)),
         self._format_search(where)))

  def __search(self, what, where, all):
    what = Path(what)
    res = []
    for root in where:
      path = root / what
      rel = root.without_prefix(drake.path_root(), force = False)
      if path.exists():
        if all:
          res.append(rel)
        else:
          return rel
      else:
        if path.absolute():
          drake_path = path.without_prefix(drake.path_root())
        else:
          drake_path = drake.path_build(path)
        node = drake.Drake.current.nodes.get(drake_path)
        if node is not None:
          if all:
            res.append(rel)
          else:
            return rel
    if len(res) > 0:
      return res
    raise Exception('Unable to find %s in %s.' % \
                    (what, self._format_search(where)))

  def _search_all(self, what, where):
    return self.__search(what, where, all = True)

  def _search_one(self, what, where):
    return self.__search(what, where, all = False)

  def _search_many(self, whats, where, all, prefer):
    res = []
    for what in whats:
      try:
        res += [(res, what) for res in self._search_all(what, where)]
        if not all and prefer is None and res:
          return res[0]
      except Exception:
        pass
    if len(res) == 0:
      raise Exception('Unable to find %s in %s.' % \
                      (self._format_search(whats),
                       self._format_search(where)))
    if not all:
      assert prefer is not None
      for prefix, what in res:
        if prefer(prefix, what):
          return prefix, what
      return res[0]
    return res

  def _search_many_all(self, whats, where, prefer = None):
    return self._search_many(whats, where,
                             all = True, prefer = prefer)

  def _search_many_one(self, whats, where, prefer = None):
    return self._search_many(whats, where,
                             all = False, prefer = prefer)

  def _format_search(self, where):
    import types
    if isinstance(where, types.GeneratorType):
      where = list(where)
    elif not isinstance(where, (list, tuple)):
      return str(where)
    if len(where) == 1:
      return str(where[0])
    return 'any of %s and %s' % (', '.join(map(str, where[:-1])),
                                 where[-1])

  def __search_version(self, what, where, major, minor, subminor):
    """ """
    if major is not None:
      if minor is not None:
        if subminor is not None:
          try:
            what.extension = 'so.%s.%s.%s' % (major, minor, subminor)
            return self._search(what, where) / what
          except Exception:
            pass
        try:
          what.extension = 'so.%s.%s' % (major, minor)
          return self._search(what, where) / what
        except Exception:
          pass
      try:
        what.extension = 'so.%s' % (major)
        return self._search(what, where) / what
      except Exception:
        pass
    what.extension = 'so'
    return self._search(what, where) / what

  def _search_lib(self, what, where, major, minor, subminor):
    """ """
    path = self.__search_version(what, where, major, minor, subminor)


class Range:

    """A numeric range."""

    def __init__(self, inf, sup = True):
        """Create a numeric range with the given boundaries

        inf -- the inferior boundary.
        sup -- the superior boundary. If unspecified, equals the
               inferior boundary. If None, there is no upper bound
               to the range (it includes any number superior or
               equal to inf).

        >>> 4 in Range(5)
        False
        >>> 5 in Range(5)
        True
        >>> 6 in Range(5)
        False

        >>> 4 in Range(5, 7)
        False
        >>> 5 in Range(5, 7)
        True
        >>> 6 in Range(5, 7)
        True
        >>> 7 in Range(5, 7)
        True
        >>> 8 in Range(5, 7)
        False
        >>> 42 in Range(5, None)
        True
        """
        if isinstance(inf, Range):
            assert sup is True
            sup = inf.sup()
            inf = inf.inf()
        assert inf is not None
        self.__inf = inf
        if sup is True:
            sup = inf
        self.__sup = sup

    def sup(self):
        return self.__sup

    def inf(self):
        return self.__inf

    def __contains__(self, val):
      """Whether val is included in self."""
      if isinstance(val, Range):
        return val.inf() in self
      sup = (self.__sup is None or val <= self.__sup)
      return val >= self.__inf and sup

    def __eq__(self, rhs):
        return self.__inf == rhs.__inf and self.__sup == rhs.__sup

    def __ge__(self, rhs):
        return self.__inf >= rhs.__sup

    def __gt__(self, rhs):
        return self.__inf > rhs.__sup

    def __str__(self):
        """A visual representation of the range.

        >>> str(Range(5))
        '5'
        >>> str(Range(5, 7))
        '[5, 7]'
        >>> str(Range(5, None))
        '[5, ...]'
        """
        if self.__sup == self.__inf:
            return str(self.__inf)
        elif self.__sup is None:
            return '[%s, ...]' % self.__inf
        return '[%s, %s]' % (self.__inf, self.__sup)

    def __repr__(self):
        if self.__sup == self.__inf:
            return 'Range(%s)' % self.__inf
        elif self.__sup is None:
            return 'Range(%s, None)' % self.__inf
        return 'Range(%s, %s)' % (self.__inf, self.__sup)

class Version:

    def __init__(self, major = None, minor = None, subminor = None):
        assert major is not None or minor is None and subminor is None
        assert minor is not None or subminor is None
        self.__major = major and Range(major)
        self.__minor = minor and Range(minor)
        self.__subminor = subminor and Range(subminor)

    @staticmethod
    def load(string):
      if string == str(Version()):
        return Version()
      v = string.split('.')[0:3]
      if len(v) == 3:
        v[2] = v[2].split('-')[0]
      return drake.Version(*[int(x) for x in v])

    @property
    def major(self):
        return self.__major

    @property
    def minor(self):
        return self.__minor

    @property
    def subminor(self):
        return self.__subminor

    def __str__(self):
        if self.__major is not None:
            if self.__minor is not None:
                if self.__subminor is not None:
                    return '%s.%s.%s' % (self.__major, self.__minor,
                                         self.__subminor)
                else:
                    return '%s.%s' % (self.__major, self.__minor)
            else:
                return '%s' % (self.__major)
        else:
            return 'any version'

    def __repr__(self):
      if self.__major is not None:
        if self.__minor is not None:
          if self.__subminor is not None:
            return 'Version(%s, %s, %s)' % (self.__major,
                                            self.__minor,
                                            self.__subminor)
          else:
            return 'Version(%s, %s)' % (self.__major, self.__minor)
        else:
          return 'Version(%s)' % (self.__major)
      else:
        return 'Version()'


    def __contains__(self, other):
      """Whether a version includes another.

      >>> Version(1, 2, 3) in Version(1, 2, 3)
      True
      >>> Version(1, 2, 2) in Version(1, 2, 3)
      False
      >>> Version(1, 2, 4) in Version(1, 2, 3)
      False
      >>> Version(1, 2) in Version(1, 2, 3)
      False
      >>> Version(1, 2, 3) in Version(1, 2)
      True
      >>> Version(1, 3) in Version(1, Range(2, 4))
      True
      >>> Version(1, 2, 3) in Version()
      True
      """
      if self.__major is not None:
        if other.__major is None or \
           not other.__major in self.__major:
          return False
        if self.__minor is not None:
          if other.__minor is None or \
             not other.__minor in self.__minor:
            return False
          if self.__subminor is not None:
            if other.__subminor is None or \
               not other.__subminor in self.__subminor:
              return False
      return True

    def __ge__(self, rhs):
        """Whether a version is greater than another.

        >>> Version(1, 2, 3) >= Version(1, 2, 3)
        True
        >>> Version(1, 2, 4) >= Version(1, 2, 3)
        True
        >>> Version(1, 3, 2) >= Version(1, 2, 3)
        True
        >>> Version(2, 0, 0) >= Version(1, 10, 23)
        True
        >>> Version(1, 2, 3) >= Version(1, 2, 4)
        False
        >>> Version(1, 2, 3) >= Version(1, 3, 2)
        False
        """
        assert self.__major is not None and rhs.__major is not None
        if self.__major == rhs.__major:
            minor = self.__minor or 0
            rhs_minor = rhs.__minor or 0
            if minor == rhs_minor:
                subminor = self.__subminor or 0
                rhs_subminor = rhs.__subminor or 0
                return subminor >= rhs_subminor
            else:
                return minor > rhs_minor
        else:
            return self.__major > rhs.__major

    def __eq__(self, rhs):
      return all(getattr(self, a) == getattr(rhs, a) for a in ('major', 'minor', 'subminor'))

    def __hash__(self):
      return hash(str(self))

class Runner(Builder):

  class Reporting(Enumerated,
                  values = ['always', 'never', 'on_failure']):
    pass

  def __init__(self,
               exe,
               args = None,
               env = None,
               stdin = None,
               prefix = None,
               targets = None,
               sources = []
  ):
    self.__args = args or list()
    self.__exe = exe
    self.__out = node('%s.out' % exe.name_relative)
    self.__err = node('%s.err' % exe.name_relative)
    self.__status = node('%s.status' % exe.name_relative)
    self.__sources = [exe] + sources
    self.__env = env
    if stdin is None:
      self.__input = None
    elif isinstance(stdin, bytes):
      self.__input = stdin
    elif isinstance(stdin, str):
      self.__input = stdin.encode('utf-8')
    else:
      raise Exception(
        'stdin must be \"str\" or \"bytes\", got \"%s\"' % type(stdin).__name__)
    self.__prefix = prefix or []
    self.stdout_reporting = Runner.Reporting.never
    self.stderr_reporting = Runner.Reporting.always
    import drake.cxx
    if isinstance(exe, drake.cxx.Executable):
        self.__sources += exe.dynamic_libraries
    Builder.__init__(
      self,
      self.__sources,
      [self.__out, self.__err, self.__status] + (targets or []))

  @property
  def status(self):
    return self.__status

  def __reporting_set(self, val):
    self.stdout_reporting = val
    self.stderr_reporting = val
  reporting = property(fget = None, fset = __reporting_set)

  def _must_report(self, reporting, status):
    if reporting is Runner.Reporting.always:
      return True
    elif reporting is Runner.Reporting.on_failure:
      return status != 0
    else:
      return False

  def _report_node(self, node):
    with open(str(node.path()), 'rb') as f:
      eol = True
      while True:
        b = f.read(4096)
        if not b:
          break
        if eol:
          eol = False
          sys.stdout.write('  ')
        for c in b:
          if c == '\n':
            eol = True
          if c > 127:
            sys.stdout.write('\\x%x' % c)
          elif c == '\\':
            sys.stdout.write('\\\\')
          else:
            sys.stdout.write(chr(c))


  def _report_node_binary(self, node):
    with open(str(node.path()), 'rb') as f:
      while True:
        b = f.read(4096)
        if not b:
          break
        sys.stdout.buffer.write(b)

  def execute(self):
    import subprocess
    def run():
      with open(str(self.__out.path()), 'w') as out, \
           open(str(self.__err.path()), 'w') as err, \
           open(str(self.__status.path()), 'w') as rv:
        if self.__env is not None:
          output_env = ('%s=%s ' % (var, pipes.quote(str(value)))
                        for var, value in self.__env.items())
        else:
          output_env = ()
        output_cmd = (pipes.quote(str(a)) for a in self.command)
        self.output(' '.join(chain(output_env, output_cmd)),
                    'Run %s' % self.__exe)
        env = dict(_OS.environ)
        if self.__env is not None:
          env.update(self.__env)
          env = { k: str(v) for k, v in env.items() }
        try:
          p = subprocess.Popen(map(str, self.command),
                               stdout = out,
                               stderr = err,
                               stdin = subprocess.PIPE,
                               env = env)
          if self.__input:
            p.communicate(self.__input)
          p.wait()
          status = p.returncode
          print(status, file = rv)
        except Exception:
          import traceback
          traceback.print_exc()
          return False
        return status
    status = self._run_job(run)
    if status is False:
      return False
    self._report(status)
    assert status is not None
    return status == 0

  def _report(self, status):
    if self._must_report(self.stdout_reporting, status):
      self._report_node(self.__out)
    if self._must_report(self.stderr_reporting, status):
      self._report_node(self.__err)

  @property
  def command(self):
    path = str(self.__exe.path())
    if not self.__exe.path().absolute():
      path = './%s' % path
    return self.__prefix + [path] + list(map(str, self.__args))

  @property
  def executable(self):
    return self.__exe

  def __str__(self):
    return str(self.__exe)

  def hash(self):
    return self.command


class TestSuite(Rule):

  def __init__(self, *args, **kwargs):
    Rule.__init__(self, *args, **kwargs)
    self.__success = 0
    self.__failures = 0

  @property
  def success(self):
    return self.__success

  @property
  def failures(self):
    return self.__failures

  @property
  def total(self):
    return self.success + self.failures

  def report_dependencies(self, deps):
    failures = []
    for dep in deps:
      if dep.build_status:
        self.__success += 1
      else:
        failures.append(dep)
        self.__failures += 1
    self._builder.output('%s: %s / %s tests passed.' %
                        (self, self.success, self.total))

  def __str__(self):
    return 'Test suite %s' % self.name()


class HTTPDownload(Builder):

  def __init__(self, url, dest, fingerprint = None,
               disable_ssl_certificate_validation = False,
               **kwargs):
    self.__urls = [url] if isinstance(url, str) else url
    self.__dest = dest
    self.__fingerprint = fingerprint
    Builder.__init__(self, [], [self.__dest])

  def execute(self):
    def job():
      self.output('Download %s to %s' % (self.__urls, self.__dest),
                  'Download %s' % self.__dest)
      import urllib.request
      response = None
      for url in self.__urls:
        try:
          response = urllib.request.urlopen(url)
        except:
          pass
        else:
          if response.status == 200:
            break
      if response is None:
        raise Exception('Unable to download %s' % self.__urls)
      return response.status, response.read()
    status, content = self._run_job(job)
    if status != 200:
      print('download failed with status %s' % status,
            file = sys.stderr)
      return False
    if self.__fingerprint is not None:
      import hashlib
      d = hashlib.md5()
      d.update(content)
      h = d.hexdigest()
      if h != self.__fingerprint:
        print('wrong checksum for %s: %s' % (self.__dest, h),
              file = sys.stderr)
        return False
    with open(str(self.__dest.path()), 'wb') as f:
      f.write(content)
    return True

  def __str__(self):
    return 'Download of %s' % self.__dest

  def __repr__(self):
    return 'HTTPDownload(%s, %s)' % (self.__urls, self.__dest)

def download(url,
             fingerprint,
             where = drake.Path('.'),
             name = None,
             disable_ssl_certificate_validation = False,
           ):
  where = drake.Path(where)
  if name is None:
    from urllib.parse import urlparse
    name = drake.Path(urlparse(url).path).basename()
    target = drake.node(where / name)
    downloader = drake.HTTPDownload(
      url,
      target,
      fingerprint = fingerprint,
      disable_ssl_certificate_validation = disable_ssl_certificate_validation,
    )
  return target


class ArchiveExtractor(Builder):

  def __init__(self, tarball, targets = [],
               patches = None, patch_dir = drake.Path('.')):
    """ Constructor
        @param patches: list of (patch_node, strip_level)
    """
    self.__tarball = tarball
    self.__patches = patches if patches is not None else ()
    self.__patch_dir = drake.Path(patch_dir)
    directory = self.__tarball.name_relative.dirname()
    self.__targets = [node(directory / target) for target in targets]
    self.__destination = self.__tarball.path().dirname()
    # targets = []
    # with tarfile.open(str(self.__tarball.path()), 'r') as f:
    #   for name in f.getnames():
    #     targets.append(directory / name)
    # for target in targets:
    #   print(target)
    # self.__targets = nodes(*targets)
    patch_nodes = map(lambda x: x[0], self.__patches)
    Builder.__init__(self, list(chain((tarball,), patch_nodes)),
                     self.__targets,
                     create_directories = False)

  @property
  def tarball(self):
    return self.__tarball

  @property
  def destination(self):
    return self.__destination

  def execute(self):
    self.output(
      'Extract %s to %s' % (self.__tarball, self.destination),
      'Extract %s' % self.__tarball)
    self._run_job(self.extract)
    for patch in self.__patches:
      if not self.cmd(
          'Apply %s' % patch[0],
          [
            'patch', '-N', '-p', str(patch[1]),
            '-d', str(self.__destination / self.__patch_dir),
            '-i', patch[0].path(absolute = True)
          ]):
        return False
    return True

  def __str__(self):
    return 'Extraction of %s' % self.__tarball

class TarballExtractor(ArchiveExtractor):

  def extract(self):
    import tarfile
    import tempfile
    # Make TarFile withable on python <= 3.1
    if not hasattr(tarfile.TarFile, '__enter__'):
      tarfile.TarFile.__enter__ = lambda self: self
    if not hasattr(tarfile.TarFile, '__exit__'):
      tarfile.TarFile.__exit__ = lambda self, v, tb, t: self.close()
    with tarfile.open(str(self.tarball.path()), 'r') as f, \
         tempfile.TemporaryDirectory(
           prefix = str(self.tarball.name().basename()) + '.',
           dir = str(self.destination)) as tmp:
      tmp = drake.Path(tmp)
      f.extractall(str(tmp))
      for f in _OS.listdir(str(tmp)):
        destination = str(drake.path_build(self.destination / f))
        if (_OS.path.exists(destination)):
          shutil.rmtree(destination)
        shutil.move(str(tmp / f), destination)

class ZipExtractor(ArchiveExtractor):

  def extract(self):
    import zipfile
    # Make TarFile withable on python <= 3.1
    if not hasattr(zipfile.ZipFile, '__enter__'):
      zipfile.ZipFile.__enter__ = lambda self: self
    if not hasattr(zipfile.ZipFile, '__exit__'):
      zipfile.ZipFile.__exit__ = lambda self, v, tb, t: self.close()
    with zipfile.ZipFile(str(self.tarball.path()), 'r') as f:
      f.extractall(str(self.destination))

def Extractor(tarball, *args, **kwargs):
  if str(tarball).endswith('.zip'):
    type = ZipExtractor
  else:
    type = TarballExtractor
  return type(tarball = tarball, *args, **kwargs)

class Zipper(Builder):

  def __init__(self, target, sources, prefix = None, whole_folder = None):
    """ Constructor
    """
    self.__target = target
    self.__sources = sources
    self.__prefix = prefix
    self.__whole_folder = whole_folder
    Builder.__init__(self, self.__sources, [self.__target])

  @property
  def target(self):
    return self.__target

  def compress(self):
    import zipfile
    with zipfile.ZipFile(str(self.__target.path()), 'w') as archive:
      if self.__whole_folder:
        import os.path
        folder = os.path.commonprefix(
          [str(source.path()) for source in self.__sources])
        assert len(folder) > 0
        for root, dirs, files in _OS.walk(str(folder)):
          for file in files:
            file = drake.Path(root) / file
            filename = file.without_prefix(self.__prefix, force = True)
            archive.write(str(file), arcname = str(filename))
      else:
        for source in self.__sources:
          source = source.path()
          filename = source.without_prefix(self.__prefix, force = True)
          archive.write(str(source), arcname = str(filename))
    return True

  def execute(self):
    self.output('Zip %s' % self.__target)
    return self._run_job(lambda: self.compress())

  def __str__(self):
    return 'Zipping of %s' % self.__target

class FileConcatenator(Builder):
  def __init__(self, target, sources):
    self.__target = target
    self.__sources = sources
    Builder.__init__(self, self.__sources, [self.__target])

  def execute(self):
    from shutil import copyfileobj
    with open(str(self.__target.path()), 'wb') as f:
      for source in self.__sources:
        with open(str(source.path()), 'rb') as s:
          copyfileobj(s, f)
    _OS.chmod(str(self.__target.path()),
              _OS.stat(str(self.__sources[0].path())).st_mode)
    return True

def host():
  system = platform.system()
  if system == 'Linux':
    os_string = 'pc-linux-gnu'
  elif system == 'Darwin':
    os_string = 'apple-darwin'
  elif platform.system() == 'Windows':
    if platform.architecture()[0] == '64bits':
      os_string = 'w64'
    else:
      os_string = 'w32'
  else:
    raise Exception('Unhandled system: %s' % system)
  return '%s-%s' % (platform.machine(), os_string)

class TemporaryDirectory:

  def __init__(self):
    self.__dir = None

  def __enter__(self):
    import tempfile
    self.__dir = tempfile.mkdtemp()
    return self

  def __exit__(self, *args, **kwargs):
    import shutil
    shutil.rmtree(self.__dir)

  def __str__(self):
    return str(self.__dir)

  @property
  def dir(self):
    return self.__dir

class PythonModule(Builder):
  '''Builder to download and extract python modules using pip3.
  '''
  def __init__(self, package_name,
               python_path,
               version = None,
               dependencies = [],
               module_name = None):
    self.__path = python_path
    self.__python_path = drake.path_build(python_path)
    self.__module_name = module_name or package_name
    self.__package_name = package_name
    self.__version = version
    self.__dependencies = dependencies
    drake.Builder.__init__(
      self,
      [],
      [drake.Node(python_path / self.__module_name / '__init__.py')])

  @property
  def root(self):
    return self.__path

  def command(self, directory = None):
    options = ['pip3', 'install',
               '--no-compile', '--install-option=--prefix=']
    if directory is not None:
      options += ['-b', directory]
    if self.__python_path is not None:
      options.append('--target=%s' % self.__python_path)
    options.append(self.__package_name)
    if self.__version:
      options[-1] += '==%s' % self.__version
    return options

  def execute(self):
    for p in self.__dependencies + [self.__module_name]:
      shutil.rmtree(str(self.__python_path / p), ignore_errors = True)
    from os import environ as os_env
    import copy
    environment = copy.deepcopy(os_env)
    if environment.get('MACOSX_DEPLOYMENT_TARGET', None):
      del environment['MACOSX_DEPLOYMENT_TARGET']
    with TemporaryDirectory() as tmp:
      return self.cmd('Installing package %s' % self.__package_name,
                      self.command(tmp.dir),
                      leave_stdout = True,
                      throw = True,
                      env = environment)
  def hash(self):
    return self.command(None)
