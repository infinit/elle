import re
from .. import Builder, Exception, Node, Path, node, srctree, CACHEDIR
from .  import Config, StaticLib, Header, Object, Source

#class MocBuilder:

deps_handler_name = 'drake.cxx.qt.moc'

class Qt:

    def __init__(self, prefix = None, gui = False):

        self.files = {}

        if prefix is None:
            test = ['/usr', '/usr/local']
        else:
            test = [prefix]

        beacon = Path('Qt/qconfig.h')
        subdirs = [Path('.'), Path('qt4')]
        lib_suffix = ''
        if platform.system() == 'Windows':
            lib_suffix = '4'
        for path in test:
            p = Path(path)
            if not p.absolute:
                p = srctree() / p
            for subdir in subdirs:
                if (p / 'include' / subdir / beacon).exists():

                    self.prefix = Path(path)

                    self.cfg = Config()
                    self.cfg.add_system_include_path(self.prefix / 'include' / subdir)
                    self.cfg.add_system_include_path(self.prefix / 'include' / subdir / 'Qt')
                    self.cfg.add_system_include_path(self.prefix / 'include' / subdir / 'QtCore')
                    self.cfg.lib_path(self.prefix / 'lib' / subdir)
                    self.cfg.lib('QtCore%s' % lib_suffix)

                    self.cfg_gui = Config()
                    self.cfg_gui.add_system_include_path(self.prefix / 'include' / subdir / 'QtGui')
                    self.cfg.lib('QtGui%s' % lib_suffix)

                    return

        raise Exception('unable to find %s in %s' % (beacon, ', '.join(test)))

    def config(self):

        return self.cfg


    def plug(self, tk):

        tk.hook_bin_deps_add(self.hook_bin_deps)
        tk.qt = self

    moc_re = re.compile('Q_OBJECT')

    def hook_bin_deps(self, linker):

        # FIXME: all_srcs should be optimized with yield
        for header in [src for src in linker.all_srcs() if src.__class__ == Header]: # FIXME: inheritance
            found = False
            for line in open(str(header), 'r'):
                if re.search(self.moc_re, line):
                    found = True
                    break
            if found:
                self.moc_file(linker, header)

    def moc_file(self, linker, header):
        p = Path(header.src_path)
        p.extension = 'moc.cc'
        src = node(p)
        if src.builder is None:
            Moc(self, header, src)
        obj_path = Path(p)
        obj_path.extension = 'o'
        obj = None
        if obj_path in Node.nodes:
            obj = node(obj_path)
        else:
            obj = Object(src, linker.toolkit, linker.config)
        linker.add_dynsrc(deps_handler_name, obj)

def deps_handler(builder, path_obj, data):

    if path_obj in Node.nodes:
        return node(path_obj)
    path_src = Path(path_obj)
    path_src.extension = 'cc'
    src = node(path_src)
    path_header = Path(path_obj)
    path_header.extension = ''
    path_header.extension = 'hh'
    header = node(path_header)
    Moc(builder.toolkit.qt, header, src)
    obj = Object(src, builder.toolkit, builder.config)
    return obj

Builder.register_deps_handler(deps_handler_name, deps_handler)
Node.extensions['moc.cc'] = Source


class Moc(Builder):

    name = 'MetaObject compilation'

    def __init__(self, qt, src, tgt):

        Builder.__init__(self, [src], [tgt])
        self.qt = qt
        self.src = src
        self.tgt = tgt

    def execute(self):

        return self.cmd('%s/bin/moc %s -o %s', self.qt.prefix, self.src, self.tgt)

