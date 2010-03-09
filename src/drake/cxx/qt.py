import re
from .. import Builder, Exception, Path, srctree
from .  import Config, StaticLib, Header, Object, Source

#class MocBuilder:



class Qt:

    def __init__(self, prefix = None, gui = False):

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

    moc_re = re.compile('Q_OBJECT')
    def hook_object_deps(self, o):

        for header in [src for src in o.builder.all_srcs() if src.__class__ == Header]: # FIXME: inheritance
            found = False
            for line in open(str(header), 'r'):
                if re.search(self.moc_re, line):
                    found = True
                    break
            if found:
                p = header.path()
                p.extension = 'moc.cc'
                src = Source(p)
                obj = Object(src, o.toolkit, o.cfg)
                o.buddy_add(obj)
                Moc(self, header, src)

class Moc(Builder):

    name = 'MetaObject compilation'

    def __init__(self, qt, src, tgt):

        Builder.__init__(self, [src], [tgt])
        self.qt = qt
        self.src = src
        self.tgt = tgt

    def execute(self):

        return self.cmd('%s/bin/moc %s -o %s', self.qt.prefix, self.src, self.tgt)
