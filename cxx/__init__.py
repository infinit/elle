from .. import ShellCommand, Builder, Node, clone, Path, node, prefix, srctree, strip_srctree, Exception, shell_escape, x86, linux, windows, strip_srctree
import os, re, shutil, subprocess, tempfile

# FIXME: Factor node and builder for executable and staticlib



class Toolkit:

    def __init__(self):

        self.includes = []

    @classmethod
    def default(self):

        return GccToolkit()

    def include(self, path):

        p = Path(path)
        if not p.absolute:
            p = srctree() / p
        self.includes.append(p)

def concatenate(chunks, prefix = ''):

    return ''.join(map(lambda v: ' %s%s' %(prefix, v), chunks))

class GccToolkit(Toolkit):

    platform = x86
    os = linux

    def object_extension(self):

        return 'o'

    def compile(self, cfg, src, obj):

        includes = ''.join(map(lambda i: ' -I %s -I %s' % (shell_escape(i), shell_escape(strip_srctree(i))), list(cfg.include_path()) + list(self.includes)))
        return 'g++%s%s -c %s -o %s' % (concatenate(cfg.flags), includes, src, obj)


    def archive(self, cfg, objs, lib):

        return 'ar crs %s %s; ranlib %s' % (lib, ' '.join(map(str, objs)), lib)


    def link(self, cfg, objs, exe):

        return 'g++ %s%s%s %s -o %s' % \
               (concatenate(cfg.flags),
                concatenate(cfg.lib_paths, '-L '),
                concatenate(cfg.libs, '-l'),
                concatenate(objs),
                exe)


    def libname(self, cfg, path):

        path = Path(path)
        return path.dirname() / ('lib%s.a' % str(path.basename()))


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

    def object_extension(self):

        return 'obj'


    def compile(self, cfg, src, obj):
        includes = ''.join(map(lambda i: ' /I %s /I %s' % (shell_escape(i), shell_escape(strip_srctree(i))), cfg.include_path() + list(self.includes)))
        return 'cl.exe /MT /TP /nologo /DWIN32 /EHsc%s /Fo%s /c %s' % (includes, obj, src)

    def archive(self, cfg, objs, lib):
        return 'lib /nologo /MT %s /OUT:%s' % (' '.join(map(str, objs)), lib)

    def link(self, cfg, objs, exe):

        # /ENTRY:main
        # /SUBSYSTEM:CONSOLE
        return 'cl.exe /nologo /MT %s /link /OUT:%s /SUBSYSTEM:WINDOWS %s %s opengl32.lib' % \
               (' '.join(map(str, objs)), exe,
                ''.join(map(lambda i : ' /LIBPATH:"%s"' %i, cfg.lib_paths)),
                ''.join(map(lambda d: ' %s.lib' % d, cfg.libs)))

    def libname(self, cfg, path):

        path = Path(path)
        return path.dirname() / ('%s.lib' % str(path.basename()))

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
        else:
            self._includes = clone(model._includes)
            self._local_includes = clone(model._local_includes)
            self._system_includes = clone(model._system_includes)
            self.lib_paths = clone(model.lib_paths)
            self.libs = clone(model.libs)
            self.flags = clone(model.flags)

    def flag(self, f):

        self.flags.append(f)

    def add_local_include_path(self, path):

        path = srctree() / prefix() / Path(path)
        self._local_includes[path] = None
        self._includes[path] = None


    def add_system_include_path(self, path):

        path = srctree() / prefix() / Path(path)
        self._system_includes[path] = None
        self._includes[path] = None


    def include_path(self):

        return list(self._includes)


    def local_include_path(self):

        return list(self._local_includes)



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
        res._includes.update(rhs._includes)
        res.lib_paths.update(rhs.lib_paths)
        res.libs.update(rhs.libs)
        res.flags += rhs.flags
        return res

class Compiler(Builder):

    name = 'C++ compilation'

    include_re = re.compile('\\s*#\\s*include\\s*(<|")(.*)(>|")')

    def __init__(self, src, obj, tk, cfg):

        self.src = src
        self.obj = obj
        self.cfg = cfg
        self.tk = tk
        Builder.__init__(self, [src], [obj])

    def dependencies(self):

        print 'mkdeps for %s' % self.src
        return self.mkdeps(self.src, 0, {})

    def execute(self):

        return self.cmd(self.tk.compile(self.cfg, self.src.path(), self.obj.path()))

    def mkdeps(self, n, lvl, marks):

        path = n.path()
        idt = ' ' * lvl * 2

        if str(path) in marks:
            return []
        marks[str(path)] = True

        # print idt, path

        res = [n]
        n.build()
        for line in open(str(path), 'r'):

            line = line.strip()
            match = self.include_re.match(line)
            if match:
                include = match.group(2)
                found = False
                for include_path in self.cfg.local_include_path():
                    test = include_path / include
                    # print idt, 'test: %s (%s)' % (test, strip_srctree(test))
                    # if re.compile('player-create').search(include):
                    #     print strip_srctree(test)
                    #     print strip_srctree(test) in Node.nodes
                    #     try:
                    #         print '=====> %s' %Node.nodes[strip_srctree(test)]
                    #     except KeyError, e:
                    #         pass
                    # FIXME: this assumes every -I $srcdir/foo has its -I $buildir/foo
                    if test.exists():
                        found = True
                        res += self.mkdeps(node(strip_srctree(test)), lvl + 1, marks)
                        break

                    test = strip_srctree(include_path) / include
                    if str(test) in Node.nodes:
                        found = True
                        res += self.mkdeps(node(test), lvl + 1, marks)
                        break
                # if not found:
                #     print idt, '=> not found: %s' % include
                #     if re.compile('player-create').search(include):
                #         print Node.nodes

        return res

class Linker(Builder):

    name = 'executable linkage'

    def __init__(self, objs, exe, tk, cfg):

        self.objs = objs
        self.exe = exe
        self.tk = tk
        self.cfg = cfg
        Builder.__init__(self, objs, [exe])

    def execute(self):

        return self.cmd(self.tk.link(self.cfg, self.objs, self.exe))

class StaticLibArchiver(ShellCommand):

    name = 'static library archiving'

    def __init__(self, objs, lib, tk, cfg):

        self.objs = objs
        self.lib = lib
        self.tk = tk
        self.cfg = cfg
        Builder.__init__(self, objs, [lib])

    def execute(self):

        return self.cmd(self.tk.archive(self.cfg, self.objs, self.lib))

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
        path = clone(source.sym_path)
        path.extension = tk.object_extension()
        Node.__init__(self, path)

        Compiler(source, self, tk, cfg)

Node.extensions['o'] = Object


class StaticLib(Node):

    def __init__(self, path, *args):

        assert len(args) == 0 or len(args) == 3

        if len(args) == 0:
            Node.__init__(self, path)
            self.builder = None
            return

        sources = args[0]
        tk = args[1]
        cfg = args[2]

        path = tk.libname(cfg, path)

        Node.__init__(self, path)

        self.sources = []

        for source in sources:

            if source.__class__ == Object:
                self.sources.append(source)
            elif source.__class__ == Source:
                o = Object(source, tk, cfg)
                self.sources.append(o)
            elif source.__class__ == StaticLib:
                # Import every object file
                for obj in source.sources:
                    self.sources.append(obj)
            else:
                raise Exception('invalid source for a static library: %s' % source)

        StaticLibArchiver(self.sources, self, tk, cfg)


class Executable(Node):

    def __init__(self, path, sources, tk, cfg):

        path = tk.exename(cfg, path)

        Node.__init__(self, path)

        self.sources = []

        for source in sources:

            if source.__class__ == Object or source.__class__ == StaticLib:
                self.sources.append(source)
            elif source.__class__ == Source:
                o = Object(source, tk, cfg)
                self.sources.append(o)
            elif source.__class__ == Header:
                pass
            else:
                self.builder = True # Hack to get the right path in error message
                raise Exception('invalid source type for executable %s: %s' % (self, source))

        Linker(self.sources, self, tk, cfg)
