import copy, os, hashlib, platform, re, sys, time


def clone(o):

    return copy.deepcopy(o)


class Exception(Exception):

    pass


DEBUG = 'DRAKE_DEBUG' in os.environ

def debug(msg):

    global DEBUG
    if DEBUG:
        print >> sys.stderr, msg

class Path:

    def __init__(self, path):

        self.absolute = False
        if path.__class__ == list:
            self.path = path
        else:
            assert path
            if platform.system() == 'Windows':
                self.path = re.split(r'/|\\', path)
                self.absolute = self.path[0] == '' or re.compile('^[A-Z]:').match(self.path[0])
            else:
                self.path = path.split('/')
                self.absolute = self.path[0] == ''

    def __getattr__(self, name):

        if name == 'extension':
            parts = self.path[-1].split('.')
            if len(parts) > 1:
                return parts[-1]
            else:
                return ''
        return object.__getattr__(self, name)

    def __setattr__(self, name, value):

        if name == 'extension':
            parts = self.path[-1].split('.')
            if len(parts) > 1:
                parts[-1] = value
                self.path[-1] = '.'.join(parts)
            else:
                self.path[-1] += '.%s' % value
        else:
            self.__dict__[name] = value
        return value

    def __str__(self):

        return '/'.join(self.path)

    def __repr__(self):

        return 'Path(\"%s\")' % str(self)

    def __lt__(self, rhs):

        return str(self) < str(rhs)

    def __hash__(self):

        return hash(str(self))

    def exists(self):

        return os.path.exists(str(self))

    def basename(self):

        return Path(self.path[-1:])

    def dirname(self):

        return Path(self.path[0:-1])

    def touch(self):

        self.dirname().mkpath()
        if not os.path.exists(str(self)):
            open(str(self), 'w').close()

    def mkpath(self):

        if not os.path.exists(str(self)):
            os.makedirs(str(self))

    def __eq__(self, rhs):

        if rhs.__class__ != Path:
            rhs = Path(rhs)
        return self.path == rhs.path

    def __div__(self, rhs):

        if self == '.':
            return clone(rhs)
        if rhs == '.':
            return clone(self)

        res = clone(self)
        if rhs.__class__ == Path:
            res.path += rhs.path
        else:
            res.path.append(rhs)
        return res

    def strip_prefix(self, rhs):

        if self.path[0:len(rhs.path)] != rhs.path:
            raise Exception("%s is not a prefix of %s" % (rhs, self))
        self.path = self.path[len(rhs.path):]
        assert self.path


class DepFile:


    CACHEDIR = '.drake'


    def __init__(self, builder):

        self.builder = builder
        builder.dsts.sort()
        self.deps = {}
        self.current_deps = {}


    def path(self):
        path = self.builder.dsts[0].path()
        return prefix() / path.dirname() / self.CACHEDIR / path.basename()


    def read(self):

        res = []

        self.path().touch()
        for line in open(str(self.path()), 'r'):
            sha1 = line[:40]
            src = node(Path(line[41:-1])) # Chomp the \n
            if str(src.path()) not in self.builder.srcs:
                res.append(src)
            self.deps[str(src.path())] = sha1
        return res

    def up_to_date(self):

        for path in self.deps:
            h = hashlib.sha1(open(path).read()).hexdigest()
            self.current_deps[path] = h
            if self.deps[path] != h:
                debug('  Execution needed because hash is outdated: %s.' % path)
                return False

        return True


    def update(self):

#        self.up_to_date()
        f = open(str(self.path()), 'w')
        for path in self.builder.srcs:
            if path in self.current_deps:
                h = self.current_deps[path]
            else:
                h = hashlib.sha1(open(path).read()).hexdigest()
            print >>f, '%s %s' % (h, self.builder.srcs[path].id())

    def __repr__(self):

        return 'DepFile(%s)' % repr(self.node)

    def __str__(self):

        return 'DepFile(%s)' % self.node



class Node:


    nodes = {}
    uid = 0
    extensions = {}


    def __init__(self, path):

        if path.__class__ == str:
            path = Path(path)

        self.sym_path = path
        self.src_path = prefix() / path

        self.uid = Node.uid
        Node.uid += 1

        self.builder = None
        self.srctree = srctree()
        Node.nodes[str(self.path())] = self


    def clean(self):

        if self.builder is not None:
            self.builder.clean()
            if self.path().exists():
                print 'Deleting %s' % self
                os.remove(str(self.path()))


    def path(self):

        if self.src_path.absolute:
            assert self.builder is None
            return self.src_path

        if self.builder is None:
            return self.srctree / self.src_path
        else:
            return self.src_path


    def id(self):

        return self.src_path


    def build(self):

        debug('Building %s' % self)
        if self.builder is None:
            if not self.path().exists():
                raise Exception('no builder to make %s' % self)
            return

        self.builder.run()


    def __setattr__(self, name, value):

        if name == 'builder' and 'builder' in self.__dict__:
            del self.nodes[str(self.path())]
            self.__dict__[name] = value
            self.nodes[str(self.path())] = self
        else:
            self.__dict__[name] = value


    def __repr__(self):

        return str(self.path())


    def __str__(self):

        return str(self.path())


    def __lt__(self, rhs):

        return self.path() < rhs.path()




def node(path):

    if path.__class__ != Path:
        path = Path(path)

    if str(path) in Node.nodes:
        return Node.nodes[str(path)]

    if path.extension not in Node.extensions:
        raise Exception('unknown file type: %s' % path)

    return Node.extensions[path.extension](path)



def nodes(*paths):

    return map(node, paths)




class Builder:


    builders = []
    uid = 0

    name = 'build'


    def __init__(self, srcs, dsts):

        assert srcs.__class__ == list
        self.srcs = {}
        self.src_list = []
        for src in srcs:
            self.add_src(src)
#        self.srcs = srcs
        self.dsts = dsts
        for dst in dsts:
            if dst.builder is not None:
                raise Exception('builder redefinition for %s' % dst)
            dst.builder = self

        self.uid = Builder.uid
        Builder.uid += 1
        Builder.builders.append(self)

        self.depfile = DepFile(self)
        self.built = False


    def dependencies(self):

        return []


    def run(self):

        if self.built:
            debug('  Already built in this run.')

        execute = False

        other_deps = self.depfile.read()
        for node in other_deps:
            if node.builder is None and not node.path().exists():
                execute = True
                debug('  Execution needed because of unkown (probably deprecated) dependency: %s.' % node)
                break

        for path in self.srcs:
            self.srcs[path].build()

        if not execute:
            for dst in self.dsts:
                if not dst.path().exists():
                    debug('  Execution needed because of missing target: %s.' % dst.path())
                    execute = True

        if not execute:
            for path in self.srcs:
                if path not in self.depfile.deps:
                    debug('  Execution needed because hash is unknown: %s.' % path)
                    execute = True
                    break

        if not execute:
            for node in other_deps:
                node.build()
            if not self.depfile.up_to_date():
                execute = True
            else:
                for node in other_deps:
                    self.add_src(node)

        if execute:

            for dep in self.dependencies():
                dep.build()
                self.add_src(dep)

            if not self.execute():
                raise Exception('%s failed' % self.name)
            for dst in self.dsts:
                if not dst.path().exists():
                    raise Exception('%s wasn\'t created by %s' % (dst, self))
            self.depfile.update()
            self.built = True
        else:
            debug('  Everything is up to date.')


    def execute(self):

        raise Exception('execute is not implemented for %s' % self)


    def clean(self):

        for path in self.srcs:
            self.srcs[path].clean()


    def __str__(self):

        return self.__class__.__name__


    def cmd(self, fmt, *args):

        rg = re.compile('\'')

        args = map(str, args)
        for arg in args:
            print arg
            if rg.match(arg):
                pass
        command = fmt % tuple(args)
        print command
        return os.system(command) == 0


    def add_src(self, src):

        self.srcs[str(src.path())] = src
        self.src_list.append(src)



class ShellCommand(Builder):

    def __init__(self, srcs, dsts, fmt, *args):

        Builder.__init__(self, srcs, dsts)
        self.fmt = fmt
        self.args = args

    def execute(self):

        return self.cmd(self.fmt, *self.args)

def shell_escape(str):

    # FIXME: escape only if needed
    # FIXME: complete that
    return '"%s"' % str

_prefix = Path('.')

def prefix():
    return _prefix

_srctree = Path('.')

def set_srctree(path):

    global _srctree
    _srctree = Path(path)

def srctree():

    global _srctree
    return _srctree

def strip_srctree(path):

    global _srctree
    res = clone(path)
    if not path.absolute:
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
    res = raw_include(str(previous_prefix / path / 'drakefile.py'), *args, **kwargs)
    _prefix = previous_prefix
    return res


def raw_include(path, *args, **kwargs):

    g = {}
    execfile(str(srctree() / path), g)
    res = Module(g)
    res.configure(*args, **kwargs)
    return res

def dot(*filters):

    def take(path):
        for filter in filters:
            if re.compile(filter).search(path):
                return False
        return True

    print 'digraph'
    print '{'
    for path in Node.nodes:
        if take(path):
            node = Node.nodes[path]
            print '  node_%s [label="%s"]' % (node.uid, node.path())
    print
    for builder in Builder.builders:
        print '  builder_%s [label="%s", shape=rect]' % (builder.uid, builder.__class__)
        for src in builder.srcs:
            if take(src):
                print '  node_%s -> builder_%s' % (builder.srcs[src].uid, builder.uid)
        for dst in builder.dsts:
            if take(str(dst.path())):
                print '  builder_%s -> node_%s' % (builder.uid, dst.uid)
    print '}'

# Architectures
x86 = 0

# OSes
linux = 0
macos = 1
windows = 2

