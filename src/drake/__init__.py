# Copyright (C) 2009-2011, Quentin "mefyl" Hocquet
#
# This software is provided "as is" without warranty of any kind,
# either expressed or implied, including but not limited to the
# implied warranties of fitness for a particular purpose.
#
# See the LICENSE file for more information.

import os, hashlib, platform, re, subprocess, sys, threading, time, types, shutil
from copy import deepcopy
import debug
from sched import Coroutine, Scheduler


class Exception(Exception):
    """Base type for any exception thrown within drake."""
    pass


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


_RAW = 'DRAKE_RAW' in os.environ
_SILENT = 'DRAKE_SILENT' in os.environ


class Path(object):

    """Node names, similar to a filesystem path."""

    separator = '/'

    if platform.system() == 'Windows':
        separator = '\\'

    def __init__(self, path):
        """Build a path.

        path -- The path, as a string or an other Path.
        """
        self.virtual = False
        self.__absolute = False
        if path.__class__ == list:
            self.__path = path
        elif path.__class__ == Path:
            self.__path = deepcopy(path.__path)
            self.__absolute = path.__absolute
        else:
            if not path:
                self.__path = []
            elif platform.system() == 'Windows':
                self.__path = re.split(r'/|\\', path)
                self.__absolute = bool(self.__path[0] == '' or re.compile('^[A-Z]:').match(self.__path[0]))
            else:
                self.__path = path.split('/')
                self.__absolute = self.__path[0] == ''

    def absolute(self):
        """Whether this path is absolute."""
        return self.__absolute

    def __extension_get(self):
        parts = self.__path[-1].split('.')
        if len(parts) > 1:
            return '.'.join(parts[1:])
        else:
            return ''

    def __extension_set(self, value):
        parts = self.__path[-1].split('.')
        if len(parts) > 1:
            if value == '':
                parts = [parts[0]]
            else:
                parts = [parts[0], value]
            self.__path[-1] = '.'.join(parts)
        else:
            if value != '':
                self.__path[-1] += '.%s' % value
        return value

    extension = property(fget = __extension_get, fset = __extension_set,
                         doc = """Extension of the file name.

                               The extension is the part after the first dot of the basename,
                               or the empty string if there are no dot.
                               """)

    def extension_strip_last_component(self):
        """Remove the last dot and what follows from the basename.

        Does nothing if there is no dot.
        """
        self.__extension_set('.'.join(self.extension.split('.')[:-1]))

    def __str__(self):
        """The path as a string, adapted to the underlying OS."""
        return (self.virtual and '//' or '') + self.separator.join(self.__path)

    def __repr__(self):
        """Python representation."""
        return 'Path(\"%s\")' % str(self)

    def __lt__(self, rhs):
        """Arbitrary comparison."""
        return str(self) < str(rhs)

    def __hash__(self):
        """Hash value."""
        return hash(str(self))

    def exists(self):
        """Whether the designed file or directory exists."""
        return os.path.exists(str(self))

    def basename(self):
        """The filename part of the path.

        This is the path without the dirname.

        >>> Path('foo/bar/baz').basename()
        Path("baz")

        Throws if the path has no components.

        >>> Path('').basename()
        Traceback (most recent call last):
        Exception: Cannot take the basename of an empty path.
        """
        if not self.__path:
            raise Exception("Cannot take the basename of an empty path.")
        return Path(self.__path[-1:])

    def dirname(self):
        """The directory part of the path.

        This is the path without the basename.

        >>> Path('foo/bar/baz').dirname()
        Path("foo/bar")
        >>> Path('foo').dirname()
        Path("")

        Throws if the path has no components.

        >>> Path('').dirname()
        Traceback (most recent call last):
        Exception: Cannot take the dirname of an empty path.
        """
        if not self.__path:
            raise Exception("Cannot take the dirname of an empty path.")
        return Path(self.__path[0:-1])

    def touch(self):
        """Create the designed file if it does not exists.

        Creates the parent directories if needed first."""
        self.dirname().mkpath()
        if not os.path.exists(str(self)):
            open(str(self), 'w').close()

    def mkpath(self):
        """Create the designed directory.

        Creates the parent directories if needed first."""
        if not os.path.exists(str(self)):
            os.makedirs(str(self))

    def __eq__(self, rhs):
        """Whether self equals rhs.

        Pathes are equals if they have the same components and absoluteness.

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
        if rhs.__class__ != Path:
            rhs = Path(rhs)
        return self.__path == rhs.__path

    def __div__(self, rhs):
        """The concatenation of self and rhs.

        rhs -- the end of the new path, as a Path or a string.

        >>> Path('foo/bar') / Path('bar/baz')
        Path("foo/bar/bar/baz")
        >>> Path('foo/bar') / 'baz'
        Path("foo/bar/baz")
        >>> Path('.') / 'baz'
        Path("baz")

        One cannot concatenate an absolute path.

        >>> Path('foo') / Path('/absolute')
        Traceback (most recent call last):
        Exception: Cannot concatenate an absolute path: Path("/absolute").
        """
        rhs = Path(rhs)
        if rhs.absolute():
            raise Exception("Cannot concatenate an absolute path: %s." % repr(rhs))

        if self == '.':
            return rhs
        if rhs == Path('.'):
            return Path(self)

        res = Path(self)
        res.__path += rhs.__path
        return res

    def strip_prefix(self, rhs):
        """Remove rhs prefix from self.

        rhs -- the prefix to strip, as a Path or a string.

        >>> p = Path('foo/bar/baz/quux')
        >>> p
        Path("foo/bar/baz/quux")
        >>> p.strip_prefix("foo/bar")
        >>> p
        Path("baz/quux")

        Throws if rhs is not a prefix of self.

        >>> p.strip_prefix("quux")
        Traceback (most recent call last):
        Exception: quux is not a prefix of baz/quux
        """
        if (not isinstance(rhs, Path)):
            rhs = Path(rhs)
        if self.__path[0:len(rhs.__path)] != rhs.__path:
            raise Exception("%s is not a prefix of %s" % (rhs, self))
        self.__path = self.__path[len(rhs.__path):]
        if not self.__path:
            self.__path = ['.']

_CACHEDIR = Path('.drake')

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
        builder.targets().sort()
        self.__files = {}
        self.__sha1 = {}


    def files(self):
        """List of hashed files."""
        return self.__files.values()


    def sha1s(self):
        """Dictonary associating sha1s to files."""
        return self.__sha1


    def register(self, node):
        """Add the node's to the hashed files."""
        self.__files[str(node.name())] = node


    def path(self):
        """Path to the file storing the hashes."""
        return self.__builder.cachedir() / self.name


    def read(self):
        """Read the hashes from the store file."""
        res = []
        self.path().touch()
        for line in open(str(self.path()), 'r'):
            sha1 = line[:40]
            remain = line[41:-1].split(' ') # Chomp the \n
            src = Path(remain[0])
            self.__sha1[str(src)] = (sha1, remain[1])

    def up_to_date(self):
        """Whether all registered files match the stored hash."""
        for path in self.__sha1.keys():
            if path not in Node.nodes:
                del self.__sha1[path]
                continue
            h = node(path).hash()
            if self.__sha1[path][0] != h:
                debug.debug('Execution needed because hash is outdated: %s.' % path, debug.DEBUG_DEPS)
                return False
        return True


    def update(self):
        """Rehash all files and write to the store file."""
        f = open(str(self.path()), 'w')
        for path in self.__files:
            h = self.__files[path].hash()
            print >>f, '%s %s %s' % (h, self.__files[path].name(), self.__files[path].drake_type())

    def __repr__(self):
        """Python representation."""
        return 'DepFile(%s)' % repr(self.__builder)

    def __str__(self):
        """String representation."""
        return 'DepFile(%s)' % self.__builder

def path_build(path):
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
    return prefix() / path

def path_src(path):
    """Return path as found in the source directory.

    This function prepend the necessary prefix to a path relative to
    the current drakefile to make it relative to the root of the source
    directory.

    This function is similar to path_build, except for the source
    directory.
    """
    return srctree() / path_build(path)

class _BaseNodeTypeType(type):

    node_types = {}

    def __call__(c, name, *arg):

        res = type.__call__(c, name, *arg)
        k = '%s.%s' % (res.__module__, res.__name__)
        _BaseNodeTypeType.node_types[k] = res
        return res

        return type.__call__(*arg)

class _BaseNodeType(type):

    __metaclass__ = _BaseNodeTypeType

    def __call__(c, *args):

        try:
            return type.__call__(c, *args)
        except NodeRedefinition, e:
            assert e.name() in BaseNode.nodes
            node = BaseNode.nodes[e.name()]
            assert node.__class__ is c
            return node


class BaseNode(object):

    """Base entity manipulated by drake.

    Nodes represent the base elements that can be built or used as
    input by the drake buildsystem. Builders have a list of sources
    nodes, which are the node they use as input when they are
    executed, and produce nodes as output.

    Nodes are often attached to file (an input file in the case of a
    source node, a generated file in the case of a target node), in
    which case its type is Node."""

    nodes = {}
    uid = 0
    extensions = {}

    __metaclass__ = _BaseNodeType

    def __init__(self, name):
        """Create a node with the given name."""
        if str(name) in BaseNode.nodes:
            raise NodeRedefinition(str(name))
        self.__name = name
        self.uid = BaseNode.uid
        BaseNode.uid += 1
        self.builder = None
        self.srctree = srctree()
        BaseNode.nodes[str(self.name())] = self
        self.consumers = []

    def name(self):
        """Node name, relative to the current drakefile."""
        res = Path(self.__name)
        res.strip_prefix(prefix())
        return res

    def name_absolute(self):
        """Node name, relative to the root of the source directory."""
        return self.__name

    def dot(self, marks):
        """Print a dot representation of this nodes build graph."""
        if (self in marks):
            return True
        marks[self] = None
        print '  node_%s [label="%s"]' % (self.uid, self.__name)
        if self.builder is not None:
            if self.builder.dot(marks):
                print '  builder_%s -> node_%s' % (self.builder.uid, self.uid)
        return True

    @classmethod
    def drake_type(self):
        """The qualified name of this type."""
        return '%s.%s' % (self.__module__, self.__name__)

    def __str__(self):
        """String representation."""
        return str(self.path())

    def hash(self):
        """Hash for this node.

        The hash value of nodes is used by builders to determine
        whether a source node has changed between two builds, and thus
        if the builder must be reexecuted.

        It must be reimplemented by subclasses.
        """
        raise Exception('hash must be implemented by BaseNodes')

    def build(self):
        """Build this node.

        Take necessary action to ensure this node is up to date. That
        is, roughly, run this node runner.
        """
        if not scheduler().running():
            c = Coroutine(self.build_coro(), name = str(self))
            scheduler().run()
        else:
            for everything in self.build_coro():
                pass

    def build_coro(self):
        """Coroutine to build this node."""
        debug.debug('Building %s.' % self, debug.DEBUG_TRACE)
        with debug.indentation():
            if self.builder is None:
                return

            if _JOBS == 1:
                for everything in self.builder.run():
                    pass
            else:
                yield self.builder.run()

    def clean(self):
        """Clean recursively for this node sources."""
        if self.builder is not None:
            self.builder.clean()

class VirtualNode(BaseNode):

    """BaseNode that do not represent a file.

    These may be configuration or meta information such as the version
    system revision, used by other nodes as sources. They are also
    used to implement Rule, which is a node that recursively builds
    other nodes, but does not directly produce a file.
    """

    def __init__(self, name):
        """Create a virtual node with the given name."""
        path = prefix() / name
        path.virtual = True
        BaseNode.__init__(self, path)

    def hash(self):
        """Virtual node children must reimplement BaseNode.hash."""
        raise Exception('hash must be implemented by VirtualNodes')


class Node(BaseNode):

    """BaseNode representing a file."""

    def __init__(self, path):
        """Construct a Node with the given path."""
        self._hash = None
        BaseNode.__init__(self, prefix() / Path(path))

    def clone(self, path):
        """Clone of this node, with an other path."""
        return Node(path)

    def hash(self):
        """Digest of the file as a string."""
        self._hash = hashlib.sha1(open(str(self.path())).read()).hexdigest()
        return self._hash

    def clean(self):
        """Clean this node's file if it is generated, and recursively its sources recursively."""
        BaseNode.clean(self)
        if self.builder is not None and self.path().exists():
            print 'Deleting %s' % self
            os.remove(str(self.path()))

    def path(self):
        """Filesystem path to node file, relative to the root of the build directory."""
        if self.name().absolute():
            # assert self.builder is None
            return self.name()
        if self.builder is None:
            return self.srctree / self.name()
        else:
            return self.name()

    def build(self):
        """Build this node."""
        if not scheduler().running():
            c = Coroutine(self.build_coro(), name = str(self))
            scheduler().run()
        else:
            for everything in self.build_coro():
                pass

    def build_coro(self):
        """Coroutine that builds this node."""
        debug.debug('Building %s.' % self, debug.DEBUG_TRACE)
        with debug.indentation():
            if self.builder is None:
                if not self.path().exists():
                    raise Exception('no builder to make %s' % self)
                return

            if _JOBS == 1:
                for everything in self.builder.run():
                    pass
            else:
                yield self.builder.run()

    def __setattr__(self, name, value):
        """Adapt the node path is the builder is changed."""
        if name == 'builder' and 'builder' in self.__dict__:
            del self.nodes[str(self.name())]
            self.__dict__[name] = value
            self.nodes[str(self.name())] = self
        else:
            self.__dict__[name] = value

    def __repr__(self):
        """Filesystem path to the node file, as a string."""
        return str(self.path())

    def __lt__(self, rhs):
        """Arbitrary global order on nodes, to enable sorting/indexing."""
        return self.path() < rhs.path()


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
    if path.__class__ != Path:
        path = Path(path)
    if str(path) in Node.nodes:
        return Node.nodes[str(path)]
    if type is not None:
        return type(path)
    if path.extension not in Node.extensions:
        return Node(path)
    return Node.extensions[path.extension](path)


def nodes(*paths):
    """Call node() on each given path and return the list of results.

    nodes('foo', 'bar', ...) is equivalent to [node('foo'), node('bar'), ...]
    """
    return map(node, paths)


def _cmd_escape(fmt, *args):
    rg = re.compile('\'')
    args = map(str, args)
    for arg in args:
        if rg.match(arg):
            pass
    return fmt % tuple(args)


def cmd(fmt, *args):
    """Expand args in fmt and run the resulting shell command.

    Expansion handles shell escaping.
    """
    command = _cmd_escape(fmt, *args)
    return os.system(command) == 0


def cmd_output(fmt, *args):
    """Expand args in fmt, run the resulting shell command and return its standard output.

    Expansion handles shell escaping.
    """
    command = _cmd_escape(fmt, *args)
    return subprocess.Popen(command, shell=True, stdout=subprocess.PIPE).communicate()[0]


class Builder:

    """Produces a set of BaseNodes from an other set of BaseNodes."""

    builders = []
    uid = 0

    name = 'build'
    _deps_handlers = {}

    @classmethod
    def register_deps_handler(self, name, f):
        """Add a dependency handler."""
        self._deps_handlers[name] = f

    def __init__(self, srcs, dsts):
        """Create a builder.

        srcs -- The list of source nodes, or the node if there is only on.
        dsts -- The list of target nodes, or the node if there is only on.
        """
        assert srcs.__class__ == list
        self.__sources = {}
        self.__vsrcs = {}
        for src in srcs:
            self.add_src(src)
#        self.__sources = srcs
        self.__targets = dsts
        for dst in dsts:
            if dst.builder is not None:
                raise Exception('builder redefinition for %s' % dst)
            dst.builder = self

        self.uid = Builder.uid
        Builder.uid += 1
        Builder.builders.append(self)

        self._depfiles = {}
        self._depfile = DepFile(self, 'drake')
        self.__built = False
        self.__built_exception = None
        self.dynsrc = {}

    def sources(self):
        """The list of source nodes."""
        return self.__sources

    def targets(self):
        """The list of target nodes."""
        return self.__targets

    def cmd(self, pretty, c, *args):
        """Run a shell command.

        pretty -- A pretty version for output.
        c      -- The command.
        args   -- Argument to expand in r

        The expansion handles shell escaping. The pretty version is
        printed, except if drake is in raw mode, in which case the
        actual command is printed.
        """
        c = _cmd_escape(c, *args)
        self.output(c, pretty)
        return cmd(c)

    def output(self, raw, pretty = None):
        """Output pretty, or raw if drake is in raw mode."""
        if not _SILENT:
            print (not _RAW and pretty) or raw


    def cachedir(self):
        """The cachedir that stores dependency files."""
        path = self.__targets[0].path()
        res = prefix() / path.dirname() / _CACHEDIR / path.basename()
        res.mkpath()
        return res


    def dependencies(self):
        """Recompute dynamic dependencies list and return them.

        Reimplemented by subclasses. This implementation returns an
        empty list.
        """
        return []


    def depfile(self, name):
        """The depfile for this node for static dependencies."""
        if name not in self._depfiles:
            self._depfiles[name] = DepFile(self, name)
        return self._depfiles[name]


    def add_dynsrc(self, name, node, data = None):
        """Add a dynamic source node."""
        self.depfile(name).register(node)
        self.dynsrc[str(node.path())] = node


    def get_type(self, tname):
        """Return the node type with the given name."""
        return _BaseNodeTypeType.node_types[tname]

    def run(self):
        """Build sources recursively, check if our target are up to date, and executed if needed."""
        global _JOBS
        debug.debug('Running %s.' % self, debug.DEBUG_TRACE_PLUS)

        # If we were already executed, just skip
        if self.__built:
            if self.__built_exception is not None:
                raise self.__built_exception
            debug.debug('Already built in this run.', debug.DEBUG_TRACE_PLUS)
            return

        # The list of static dependencies is now fixed
        for path in self.__sources:
            self._depfile.register(self.__sources[path])

        # See Whether we need to execute or not
        execute = False

        # Reload dynamic dependencies
        if not execute:
            for f in os.listdir(str(self.cachedir())):
                if f == 'drake':
                    continue
                debug.debug('Considering dependencies file %s' % f, debug.DEBUG_DEPS)
                depfile = self.depfile(f)
                depfile.read()
                handler = self._deps_handlers[f]

                with debug.indentation():
                    for path in depfile.sha1s():

                        if path in self.__sources or path in self.dynsrc:
                            debug.debug('File %s is already in our sources.' % path, debug.DEBUG_DEPS)
                            continue

                        if path in Node.nodes:
                            node = Node.nodes[path]
                        else:
                            debug.debug('File %s is unknown, calling handler.' % path, debug.DEBUG_DEPS)
                            node = handler(self, path, self.get_type(depfile.sha1s()[path][1]), None)

                        debug.debug('Adding %s to our sources.' % node, debug.DEBUG_DEPS)
                        self.add_dynsrc(f, node, None)


        coroutines = []

        # Build static dependencies
        debug.debug('Build static dependencies')
        with debug.indentation():
            for node in self.__sources.values() + self.__vsrcs.values():
                if node.builder is None or \
                        node.builder.__built:
                    continue
                if _JOBS == 1:
                    for everything in node.build_coro():
                        pass
                else:
                    coroutines.append(Coroutine(node.build_coro(), str(node), scheduler()))

        # Build dynamic dependencies
        debug.debug('Build dynamic dependencies')
        with debug.indentation():
            for path in self.dynsrc:
                try:
                    node = self.dynsrc[path]
                    if node.builder is None or \
                            node.builder.__built:
                        continue
                    if _JOBS == 1:
                        for everything in node.build_coro():
                            pass
                    else:
                        coroutines.append(Coroutine(node.build_coro(), str(node), scheduler()))
                except Exception, e:
                    debug.debug('Execution needed because dynamic dependency couldn\'t be built: %s.' % path)
                    execute = True

        if _JOBS != 1:
            for coro in coroutines:
                yield coro

        # If any target is missing, we must rebuild.
        if not execute:
            for dst in self.__targets:
                if not dst.path().exists():
                    debug.debug('Execution needed because of missing target: %s.' % dst.path(), debug.DEBUG_DEPS)
                    execute = True

        # Load static dependencies
        self._depfile.read()

        # If a new dependency appeared, we must rebuild.
        if not execute:
            for p in self.__sources:
                path = self.__sources[p].name()
                if path not in self._depfile.sha1s():
                    debug.debug('Execution needed because a new dependency appeared: %s.' % path, debug.DEBUG_DEPS)
                    execute = True
                    break

        # Check if we are up to date wrt all dependencies
        if not execute:
            if not self._depfile.up_to_date():
                execute = True
            for f in self._depfiles:
                if not self._depfiles[f].up_to_date():
                    execute = True


        if execute:
            debug.debug('Executing builder %s' % self, debug.DEBUG_TRACE)

            # Regenerate dynamic dependencies
            self.dynsrc = {}
            self._depfiles = {}
            debug.debug('Recomputing dependencies', debug.DEBUG_TRACE_PLUS)
            with debug.indentation():
                self.dependencies()

            debug.debug('Rebuilding new dynamic dependencies', debug.DEBUG_TRACE_PLUS)
            with debug.indentation():
                for node in self.dynsrc.values():
                    for y in node.build_coro():
                        yield y

            if not self.execute():
                self.__built = True
                self.__built_exception = Exception('%s failed' % self)
                raise self.__built_exception
            for dst in self.__targets:
                if not dst.path().exists():
                    raise Exception('%s wasn\'t created by %s' % (dst, self))
            self._depfile.update()
            for name in self._depfiles:
                self._depfiles[name].update()
            self.__built = True
        else:
            self.__built = True
            debug.debug('Everything is up to date.', debug.DEBUG_TRACE_PLUS)


    def execute(self):
        """Generate target nodes from source node.

        Must be reimplemented by subclasses.
        """
        raise Exception('execute is not implemented for %s' % self)


    def clean(self):
        """Clean source nodes recursively."""
        for path in self.__sources:
            self.__sources[path].clean()


    def __str__(self):
        """String representation."""
        return self.__class__.__name__


    def add_src(self, src):
        """Add a static source."""
        self.__sources[str(src.name())] = src
        src.consumers.append(self)


    def add_virtual_src(self, src):
        """Add a virtual source.

        Virtual sources are built when the builder is runned, but are
        not taken in account when determining if this builder must be
        executed.
        """
        self.__vsrcs[str(src.path())] = src


    def all_srcs(self):
        """All sources, recursively."""
        res = []
        for src in self.__sources.values() + self.dynsrc.values():
            res.append(src)
            if src.builder is not None:
                res += src.builder.all_srcs()
        return res

    def dot(self, marks):
        """Print a dot representation of the build graph."""
        if (self in marks):
            return True
        marks[self] = None

        print '  builder_%s [label="%s", shape=rect]' % (self.uid, self.__class__)
        for node in self.__sources.values() + self.dynsrc.values():
            if node.dot(marks):
                print '  node_%s -> builder_%s' % (node.uid, self.uid)
        return True


class ShellCommand(Builder):

    def __init__(self, srcs, dsts, fmt, *args):

        Builder.__init__(self, srcs, dsts)
        self.fmt = fmt
        self.args = args

    def execute(self):

        return self.cmd(self.fmt, *self.args)


class Dictionary(VirtualNode):

    def __init__(self, name, content = {}):

        VirtualNode.__init__(self, 'dictionaries/%s' % name)
        self.content = content

    def hash(self):

        # FIXME: sha1 of the string repr ain't optimal
        items = self.content.items()
        items.sort()
        return hashlib.sha1(str(items)).hexdigest()

    def __iter__(self):

        return iter(self.content.items())


class Expander(Builder):

    def __init__(self, dicts, sources, target, matcher, missing_fatal):

        if not isinstance(dicts, list):
            dicts = [dicts]

        Builder.__init__(self, sources + dicts, [target])
        self.__dicts = dicts
        self.matcher = matcher
        self.missing_fatal = missing_fatal
        self.__target = target

    def execute(self):

        self.output('Expand %s' % (self.__target))

        vars = {}
        for d in self.__dicts:
            vars.update(dict(d))
        content = self.content()
        for match in self.matcher.finditer(content):
            key = match.group(1)
            try:
                content = content.replace(match.group(0), str(vars[key]))
            except KeyError:
                if self.missing_fatal:
                    print 'Missing expansion: %s' % key
                    return False

        print >> open(str(self.__target.path()), 'w'), content
        return True

    def dictionaries(self):

        return self.__dicts

    def target(self):
        return self.__target

class FileExpander(Expander):

    def __init__(self, dicts, source, target = None,
                 matcher = re.compile('@([a-zA-Z0-9_-]+)@'),
                 missing_fatal = True):

        self.__source = source
        if target is None:
            target = Path(source.name())
            target.extension_strip_last_component()
            target = node(target)
        else:
            assert isinstance(target, BaseNode)

        Expander.__init__(self,
                          dicts = dicts,
                          sources = [source],
                          target = target,
                          matcher = matcher,
                          missing_fatal = missing_fatal)

    def content(self):

        return open(str(self.__source.path()), 'r').read()

    def source(self):

        return self.__source


class TextExpander(Expander):

    def __init__(self, dicts, text, target,
                 matcher = re.compile('@([a-zA-Z0-9_-]+)@'),
                 missing_fatal = True):

        self.__text = text
        assert isinstance(target, BaseNode)

        Expander.__init__(self,
                          dicts = dicts,
                          sources = [],
                          target = target,
                          matcher = matcher,
                          missing_fatal = missing_fatal)

    def content(self):

        return self.__text;

    def text(self):

        return self.__text

class FunctionExpander(Expander):

    def __init__(self, dicts, function, target,
                 matcher = re.compile('@([a-zA-Z0-9_-]+)@'),
                 missing_fatal = True):

        self.__function = function
        assert isinstance(target, BaseNode)

        Expander.__init__(self,
                          dicts = dicts,
                          sources = [],
                          target = target,
                          matcher = matcher,
                          missing_fatal = missing_fatal)

    def content(self):

        res = ''
        for d in self.dictionaries():
            for key, value in d:
                res += self.__function(key, value)
        return res

    def function(self):

        return self.__function

def shell_escape(s):

    # FIXME: escape only if needed
    # FIXME: complete that
    return '"%s"' % str(s).replace('"', '\\"')

_prefix = Path('')

def prefix():
    return _prefix

_srctree = Path('')

def set_srctree(path):

    global _srctree
    _srctree = Path(path)

def srctree():

    global _srctree
    return _srctree

def strip_srctree(path):

    global _srctree
    res = Path(path)
    if not path.absolute():
        res.strip_prefix(_srctree)
    return res

class Module:

    def __init__(self, globals):

        self.globals = globals

    def __getattr__(self, name):

        return self.globals[name]


def include(path, *args, **kwargs):

    global _prefix

    path = Path(path)
    previous_prefix = _prefix
    _prefix = _prefix / path
    drakefile = None
    names = ['drakefile', 'drakefile.py']
    for name in names:
        if path_src(name).exists():
            drakefile = previous_prefix / path / name
            break
    if drakefile is None:
        raise Exception('cannot find %s or %s in %s' % (', '.join(names[:-1]), names[-1], path))
    res = raw_include(str(drakefile), *args, **kwargs)
    _prefix = previous_prefix
    return res


def raw_include(path, *args, **kwargs):

    g = {}
    execfile(str(srctree() / path), g)
    res = Module(g)
    res.configure(*args, **kwargs)
    return res

def dot(node, *filters):

    # FIXME: coro!
    node.build()
    marks = {}
    print 'digraph'
    print '{'
    node.dot(marks)
    print '}'

_MODES = {}

def all_if_none(nodes):

    # Copy it, since it will change during iteration. This shouldn't
    # be a problem, all newly inserted will be dependencies of the
    # already existing nodes. Right?
    if len(nodes):
        return nodes
    else:
        return list(Node.nodes.values())

def command_add(name, action):
    _MODES[name] = action

def build(nodes):
    if not len(nodes):
        nodes = [node for node in Node.nodes.values() if not len(node.consumers)]
    if _JOBS == 1:
        for node in all_if_none(nodes):
            for everything in node.build_coro():
                pass
    else:
        for node in all_if_none(nodes):
            Coroutine(node.build_coro(), str(node), scheduler())
        scheduler().run()
command_add('build', build)

def clean(nodes):
    for node in all_if_none(nodes):
        node.clean()
command_add('clean', clean)

def dot_cmd(nodes):
    for node in all_if_none(nodes):
        dot(node)
command_add('dot', dot_cmd)

def dot_show_cmd(nodes):
    if not len(nodes):
        print '%s: dot-show: give me some nodes to show.' % sys.argv[0]
    for node in nodes:
        p = subprocess.Popen('dot -Tpng | xv -', shell = True, stdin = subprocess.PIPE)
        stdout = sys.stdout
        sys.stdout = p.stdin
        dot(node)
        p.communicate()
        sys.stdout = stdout

command_add('dot-show', dot_show_cmd)

_JOBS = 1
_SCHEDULER = None

def scheduler():
    global _JOBS, _SCHEDULER
    if _SCHEDULER is None:
        _SCHEDULER = Scheduler(_JOBS)
    return _SCHEDULER

def jobs_set(n):
    global _JOBS, _SCHEDULER
    assert _SCHEDULER is None
    _JOBS = int(n)

_OPTIONS = {
    '--jobs': jobs_set,
    '-j'    : jobs_set,
}

def run(root, *cfg, **kwcfg):

    try:

        print '%s: Entering directory `%s\'' % (sys.argv[0], os.getcwd())
        set_srctree(root)
        root = raw_include('drakefile', *cfg, **kwcfg)

        args = sys.argv[1:]

        mode = _MODES['build']
        i = 0

        while True:

            if i < len(args):
                arg = args[i]

                if arg in _OPTIONS:
                    _OPTIONS[arg](args[i + 1])
                    i += 2
                    continue

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
            mode(nodes)

            if i == len(args):
                break

    except Exception, e:
        print '%s: %s' % (sys.argv[0], e)
        exit(1)
    except KeyboardInterrupt:
        print '%s: interrupted.' % sys.argv[0]
        exit(1)
    print '%s: Leaving directory `%s\'' % (sys.argv[0], os.getcwd())


class Copy(Builder):

    def __init__(self, fr, to, strip_prefix = None):

        self.__from = fr
        stripped = Path(fr.name())
        if strip_prefix is not None:
            stripped.strip_prefix(strip_prefix)
        self.__to = fr.clone(Path(to) / stripped)
        self.__to.builder = None
        Builder.__init__(self, [self.__from], [self.__to])

    def to(self):

        return self.__to

    def execute(self):

        self.output('Copy %s to %s' % (self.__from, self.__to),
                    'Copy %s' % self.__to,)
        # FIXME: errors!
        shutil.copyfile(str(self.__from), str(self.__to))
        return True


def copy(fr, to, strip_prefix = None):

    if isinstance(fr, list):
        res = []
        for node in fr:
            res.append(copy(node, to, strip_prefix))
        return res
    else:
        return Copy(fr, to, strip_prefix = strip_prefix).to()

class Rule(VirtualNode):

    def __init__(self, name, nodes = []):

        VirtualNode.__init__(self, name)
        class RuleBuilder(Builder):
            def run(self):
                debug.debug('Build static dependencies')
                coroutines = []
                with debug.indentation():
                    for node in self.sources().values():
                        if _JOBS == 1:
                            for everything in node.build_coro():
                                pass
                        else:
                            coroutines.append(Coroutine(node.build_coro(), name = str(node)))
                if _JOBS != 1:
                    for coro in coroutines:
                        yield coro

        RuleBuilder([], [self])

    def hash(self):

        return ''

    def __lshift__(self, nodes):

        if isinstance(nodes, list):
            for node in nodes:
                self << node
        else:
            self.builder.add_src(nodes)


def re_map(f, r, s):

  match = re.search(r, s)
  while match:
    prefix = s[:match.span()[0]]
    suffix = s[match.span()[1]:]
    s = '%s%s%s' % (prefix, f(match.group()), suffix)
    match = re.search(r, s)
  return s

def camel_case(s):

  return re_map(lambda s: s[1].capitalize(),
                re.compile('-[a-zA-Z]'), s)

# Architectures
x86 = 0

# OSes
android = 0
linux = 1
macos = 2
windows = 3

