import re
from .. import ShellCommand, Builder, Node, clone, Path, node, prefix, srctree, strip_srctree, Exception, shell_escape, x86, linux, windows, strip_srctree, cmd, command_add, debug

# FIXME: Factor node and builder for executable and staticlib



class Toolkit:

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

    # def include(self, path):

    #     p = Path(path)
    #     if not p.absolute:
    #         p = srctree() / p
    #     self.includes.append(p)

def concatenate(chunks, prefix = ''):

    return ''.join(map(lambda v: ' %s%s' %(prefix, v), chunks))

class GccToolkit(Toolkit):

    platform = x86
    os = linux

    def __init__(self, compiler = 'g++'):

        Toolkit.__init__(self)
        self.cxx = compiler

    def object_extension(self):

        return 'o'

    def cppflags(self, cfg):

        def print_define((name, v)):
            if v is None:
                return '-D%s' % name
            else:
                return '-D%s=%s' % (name, v)
        defines = ' '.join(map(print_define, cfg.defines().items()))
        system_includes = ' '.join(map(lambda i: '-I %s' % shell_escape(i), cfg.system_include_path()))
        local_includes  = ' '.join(map(lambda i: '-I %s -I %s' % (shell_escape(i), shell_escape(strip_srctree(i))), cfg.local_include_path()))
        return ' '.join([system_includes, local_includes, defines])

    def compile(self, cfg, src, obj):

        return ' '.join([self.cxx] + cfg.flags + [self.cppflags(cfg), '-c', str(src), '-o', str(obj)])


    def archive(self, cfg, objs, lib):

        return 'ar crs %s %s; ranlib %s' % (lib, ' '.join(map(str, objs)), lib)


    def link(self, cfg, objs, exe):

        return '%s %s%s%s%s %s -o %s' % \
               (self.cxx,
                concatenate(cfg.flags),
                concatenate(cfg.frameworks(), '-framework '),
                concatenate(cfg.lib_paths, '-L'),
                concatenate(cfg.libs, '-l'),
                concatenate(objs),
                exe)

    def dynlink(self, cfg, objs, exe):

        return '%s %s%s%s%s %s -shared -o %s' % \
               (self.cxx,
                concatenate(cfg.flags),
                concatenate(cfg.frameworks(), '-framework '),
                concatenate(cfg.lib_paths, '-L'),
                concatenate(cfg.libs, '-l'),
                concatenate(objs),
                exe)


    def libname_static(self, cfg, path):

        path = Path(path)
        return path.dirname() / ('lib%s.a' % str(path.basename()))

    def libname_dyn(self, cfg, path):

        path = Path(path)
        return path.dirname() / ('lib%s.so' % str(path.basename()))


    def exename(self, cfg, path):

        return Path(path)


class VisualToolkit(Toolkit):

    platform = x86
    os = windows

    def __init__(self):

        Toolkit.__init__(self)

        def output(cmd):

            return subprocess.Popen(cmd, shell = True, stdout = subprocess.PIPE).stdout.read().strip()

        # Des barres de rire cet OS j'vous dit.
        cfg = output("echo %VS90COMNTOOLS%")
        if not cfg:
            raise Exception("VS90COMNTOOLS is not defined, check your Visual Studio installation.")
        cfg = Path(cfg)
        if not cfg.exists():
            raise Exception("Visual Studio configuration file does not exist: %s" % cfg)
        cfg = cfg / 'vsvars32.bat'
        tmp = Path(tempfile.mkdtemp())
        cp = tmp / 'fuck.bat'
        shutil.copy(str(cfg), str(cp))
        f = open(str(cp), 'a')
        print >> f, '@echo %PATH%' # Des putain de barres.
        print >> f, '@echo %LIB%'
        print >> f, '@echo %INCLUDE%'
        f.close()
        res = output('"%s"' % cp).split('\r\n')
        path = res[-3]
        lib = res[-2]
        include = res[-1]
        os.environ['PATH'] = path
        os.environ['LIB'] = lib
        os.environ['INCLUDE'] = include
        shutil.rmtree(str(tmp))
        self.flags = []

    def warning_disable(self, n):
        self.flags.append('/wd%s' % n)

    def object_extension(self):
        return 'obj'

    def compile(self, cfg, src, obj):
        includes = ''.join(map(lambda i: ' /I %s /I %s' % (shell_escape(i), shell_escape(strip_srctree(i))), cfg.include_path() + list(self.includes)))
        return 'cl.exe /MT /TP /nologo /DWIN32 %s %s /EHsc%s /Fo%s /c %s' % (' '.join(self.flags), concatenate(cfg.flags), includes, obj, src)

    def archive(self, cfg, objs, lib):
        return 'lib /nologo /MT %s /OUT:%s' % (' '.join(map(str, objs)), lib)

    def link(self, cfg, objs, exe):

        # /ENTRY:main
        # /SUBSYSTEM:CONSOLE
        return 'cl.exe /nologo /MT %s %s /link /OUT:%s /SUBSYSTEM:WINDOWS %s %s opengl32.lib' % \
               (' '.join(cfg.flags),
                ' '.join(map(str, objs)),
                exe,
                ''.join(map(lambda i : ' /LIBPATH:"%s"' %i, cfg.lib_paths)),
                ''.join(map(lambda d: ' %s.lib' % d, cfg.libs)))

    def libname_static(self, cfg, path):

        path = Path(path)
        return path.dirname() / ('%s.lib' % str(path.basename()))

    def libname_dyn(self, cfg, path):

        path = Path(path)
        return path.dirname() / ('%s.dll' % str(path.basename()))

    def exename(self, cfg, path):

        res = Path(path)
        res.extension = 'exe'
        return res


class Config:

    def __init__(self, model = None):

        if model is None:
            self._includes = {}
            self._local_includes = {}
            self._system_includes = {}
            self.lib_paths = {}
            self.libs = {}
            self.flags = []
            self._framework = {}
            self._defines = {}
        else:
            self._includes = clone(model._includes)
            self._local_includes = clone(model._local_includes)
            self._system_includes = clone(model._system_includes)
            self.lib_paths = clone(model.lib_paths)
            self.libs = clone(model.libs)
            self.flags = clone(model.flags)
            self._framework = clone(model._framework)
            self._defines = clone(model._defines)

    def define(self, name, value = None):

        self._defines[name] = value

    def defines(self):
        return self._defines

    def flag(self, f):

        self.flags.append(f)

    def framework_add(self, name):

        self._framework[name] = None

    def frameworks(self):

        return self._framework.keys()

    def add_local_include_path(self, path):

        path = srctree() / prefix() / Path(path)
        self._local_includes[path] = None
        self._includes[path] = None


    def add_system_include_path(self, path):

        path = Path(path)
        if not path.absolute:
            path = srctree() / prefix() / path
        self._system_includes[path] = None
        self._includes[path] = None


    def include_path(self):

        return list(self._includes)


    def local_include_path(self):

        return list(self._local_includes)


    def system_include_path(self):

        return list(self._system_includes)


    def lib_path(self, path):

        p = Path(path)
        if not p.absolute:
            p = srctree() / prefix() / p
        self.lib_paths[p] = None


    def lib(self, lib):

        self.libs[lib] = None


    def __add__(self, rhs):

        res = Config(self)
        res._local_includes.update(rhs._local_includes)
        res._system_includes.update(rhs._system_includes)
        res._includes.update(rhs._includes)
        res._framework.update(rhs._framework)
        res.lib_paths.update(rhs.lib_paths)
        res.libs.update(rhs.libs)
        res.flags += rhs.flags
        return res

def deps_handler(builder, path, t, data):

    return node(path, t)


def mkdeps(n, lvl, config, marks,
           f_submarks, f_init, f_add):

    include_re = re.compile('\\s*#\\s*include\\s*(<|")(.*)(>|")')

    path = n.path()
    idt = ' ' * lvl * 2

    if str(path) in marks:
        return []
    marks[str(path)] = True

#    debug('%smkdeps: %s' % (idt, path))

    res = f_init(n)

    def unique(include, prev, new):

        if prev is not None:
            raise Exception('two node match inclusion %s: %s and %s' % (include, prev, new))
        return new


    n.build()
    for line in open(str(path), 'r'):

        line = line.strip()
        match = include_re.match(line)
        if match:
            include = match.group(2)
            found = None
            for include_path in config.local_include_path():

                test = strip_srctree(include_path) / include
                if str(test) in Node.nodes:
                    # Check this is not an old cached dependency from cxx.inclusions.
                    # Not sure of myself though.
                    if test.exists() or node(str(test)).builder is not None:
                        found = unique(include, found, node(test))
#                        debug('%sfound node: %s' % (idt, test))


                test = include_path / include
                # FIXME: this assumes every -I $srcdir/foo has its -I $buildir/foo
                if test.exists():
#                    debug('%sfound file: %s' % (idt, test))
                    found = unique(include, found, node(strip_srctree(test), Header))

            if found is not None:
                f_add(res, found, mkdeps(found, lvl + 1, config, f_submarks(marks), f_submarks, f_init, f_add))

    return res


class Compiler(Builder):

    name = 'C++ compilation'
    deps = 'drake.cxx.inclusions'

    Builder.register_deps_handler(deps, deps_handler)

    def __init__(self, src, obj, tk, cfg):

        self.src = src
        self.obj = obj
        self.config = cfg
        self.toolkit = tk
        Builder.__init__(self, [src], [obj])


    def dependencies(self):

        debug('dependencies')

        deps = mkdeps(self.src, 0, self.config, {},
                      f_init = lambda n: [n],
                      f_submarks = lambda d: d,
                      f_add = lambda res, node, sub: res.extend(sub))

        for dep in deps:
            if dep != self.src:
                self.add_dynsrc(self.deps, dep)
        for hook in self.toolkit.hook_object_deps():
            hook(self)


    def execute(self):

        return self.cmd('Compile %s' % self.obj, self.toolkit.compile(self.config, self.src.path(), self.obj.path()))

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


    def dependencies(self):

        for hook in self.toolkit.hook_bin_deps():
            hook(self)

    def __init__(self, objs, exe, tk, cfg):

        self.exe = exe
        self.toolkit = tk
        self.config = cfg
        # This duplicates self.srcs, but preserves the order.
        self.objs = objs
        Builder.__init__(self, objs, [exe])


    def execute(self):

        return self.cmd('Link %s' % self.exe, self.toolkit.link(self.config, self.objs + self.dynsrc.values(), self.exe))

    def __repr__(self):

        return 'Linker(%s)' % self.exe

    def __str__(self):

        return 'Linker(%s)' % self.exe


class DynLibLinker(Builder):


    name = 'dynamic library linkage'


    def dependencies(self):

        for hook in self.toolkit.hook_bin_deps():
            hook(self)

    def __init__(self, objs, lib, tk, cfg):

        self.lib = lib
        self.toolkit = tk
        self.config = cfg
        # This duplicates self.srcs, but preserves the order.
        self.objs = objs
        Builder.__init__(self, objs, [lib])


    def execute(self):

        return self.cmd('Link %s' % self.lib, self.toolkit.dynlink(self.config, self.objs + self.dynsrc.values(), self.lib))

    def __repr__(self):

        return 'Linker for %s' % self.lib


class StaticLibLinker(ShellCommand):

    name = 'static library archiving'

    def __init__(self, objs, lib, tk, cfg):

        self.objs = objs
        self.lib = lib
        self.tk = tk
        self.cfg = cfg
        Builder.__init__(self, objs, [lib])

    def execute(self):

        return self.cmd('Link %s' % self.lib, self.tk.archive(self.cfg, self.objs, self.lib))

class Source(Node):

    def __init__(self, path):

        Node.__init__(self, path)

Node.extensions['c'] = Source
Node.extensions['cc'] = Source

class Header(Node):

    def __init__(self, path):

        Node.__init__(self, path)

Node.extensions['h'] = Header
Node.extensions['hh'] = Header
Node.extensions['hpp'] = Header
Node.extensions['hxx'] = Header


class Object(Node):

    def __init__(self, source, tk, cfg):

        self.source = source
        self.toolkit = tk
        self.cfg = cfg
        path = clone(source.sym_path)
        path.extension = tk.object_extension()
        Node.__init__(self, path)

        Compiler(source, self, tk, cfg)

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

        for source in sources:
            self.src_add(source, self.tk, self.cfg)


    def src_add(self, source, tk, cfg):

        if source.__class__ == Object:
            self.sources.append(source)
        elif source.__class__ == Source:
            # FIXME: factor
            p = Path(source.sym_path)
            p.extension = 'o'
            if str(p) in Node.nodes:
                o = Node.nodes[p]
            else:
                o = Object(source, tk, cfg)
            self.sources.append(o)
        elif source.__class__ == Header:
            pass
        elif source.__class__ == StaticLib:
            # Import every object file
            for obj in source.sources:
                self.sources.append(obj)
        else:
            for hook in tk.hook_bin_src():
                res = hook(source)
                if res is not None:
                    self.src_add(res, tk, cfg)
            obj = source.produced_direct()
            if obj is None:
                raise Exception('invalid source for a library: %s' % source)
            self.src_add(obj, tk, cfg)

class DynLib(Binary):

    def __init__(self, path, sources, tk, cfg):

        Binary.__init__(self, tk.libname_dyn(cfg, path), sources, tk, cfg)
        DynLibLinker(self.sources, self, self.tk, self.cfg)

class StaticLib(Binary):

    def __init__(self, path, sources, tk, cfg):

        Binary.__init__(self, tk.libname_static(cfg, path), sources, tk, cfg)
        StaticLibLinker(self.sources, self, self.tk, self.cfg)

class Executable(Binary):

    def __init__(self, path, sources, tk, cfg):

        path = tk.exename(cfg, path)
        Binary.__init__(self, path, sources, tk, cfg)
        Linker(self.sources, self, self.tk, self.cfg)

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
        return [node for node in Node.nodes.values() if node.__class__ == Object]

def dot_merge(nodes):

    def rec(n, d, marks = {}, skip = False):
        if not skip:
            print '  node_%s [label="%s"]' % (n.uid, n.sym_path)
        for s in d:
            rec(s, d[s], marks)
            k = (n.uid, s.uid)
            if k not in marks:
                marks[k] = None
                print '  node_%s -> node_%s' % k

    print 'digraph'
    print '{'
    deps = deps_merge(all_objects_if_none(nodes))
    marks = {}
    print '  {'
    print '    rank=same'
    for n in deps:
        print '    node_%s [label="%s"]' % (n.uid, n.sym_path)
    print '  }'
    for n in deps:
        rec(n, deps[n], marks, True)
    print '}'

def dot_spread(nodes):

    def rec(n, d, i = 0):
        me = i
        print '  node_%s [label="%s"]' % (me, n.sym_path)
        for s in d:
            si = i + 1
            i = rec(s, d[s], si)
            k = (s.uid, n.uid)
            print '  node_%s -> node_%s' % (me, si)
        return i


    print 'digraph'
    print '{'
    deps = deps_merge(all_objects_if_none(nodes))
    i = 0
    for n in deps:
        i = rec(n, deps[n], i) + 1
    print '}'

command_add('cxx-deps-dot-merge', dot_merge)
command_add('cxx-deps-dot', dot_spread)
