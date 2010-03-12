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

        for path in test:
            p = Path(path)
            if not p.absolute:
                p = srctree() / p
            if (p / 'include/qt4/Qt/qconfig.h').exists():

                self.prefix = path

                self.cfg = Config()
                self.cfg.add_system_include_path('%s/include/qt4' % self.prefix)
                self.cfg.add_system_include_path('%s/include/qt4/Qt' % self.prefix)
                self.cfg.add_system_include_path('%s/include/qt4/QtCore' % self.prefix)
                self.cfg.lib_path('%s/lib/qt4' % self.prefix)
                self.cfg.lib('QtCore')
                self.cfg.lib('QtGui')

                self.cfg_gui = Config()
                self.cfg_gui.add_system_include_path('%s/include/qt4/QtGui' % self.prefix)



                return

        raise Exception('unable to find boost/version.hpp in %s' % ', '.join(test))

    def config(self):

        return self.cfg


    def plug(self, tk):

        tk.hook_object_deps_add(self.hook_object_deps)
        tk.qt = self

    moc_re = re.compile('Q_OBJECT')
    def hook_object_deps(self, compiler):

        for header in [src for src in compiler.all_srcs() if src.__class__ == Header]: # FIXME: inheritance
            found = False
            for line in open(str(header), 'r'):
                if re.search(self.moc_re, line):
                    found = True
                    break
            if found:
                self.moc_file(compiler, header)

    def moc_file(self, compiler, header):
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
            obj = Object(src, compiler.toolkit, compiler.config)
        compiler.obj.buddy_add(deps_handler_name, obj)

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
    buddy = Object(src, builder.toolkit, builder.config)
#    builder.obj.buddy_add(deps_handler_name, buddy)
    return buddy

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

