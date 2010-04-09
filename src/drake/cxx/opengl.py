import platform
from .. import Path, Exception, srctree
from .  import Config, StaticLib

class OpenGL:

    def __init__(self, prefix = None):

        test = []
        test_framework = []
        if prefix is None:
            test = ['/usr', '/usr/local']
            if platform.system() == 'Darwin':
                test_framework += ['/System/Library/Frameworks/OpenGL.framework']
        else:
            test = [prefix]
            if platform.system() == 'Darwin':
                test_framework += [prefix]

        for path in test:

            p = Path(path)
            if not p.absolute:
                p = srctree() / p

            if (p / 'include/GL/gl.hh').exists():

                self.prefix = path

                self.cfg = Config()
                self.cfg.add_system_include_path('%s/include' % self.prefix)
                self.cfg.lib_path('%s/lib' % self.prefix)
                self.cfg.lib('GL')

                return

        if platform.system() == 'Darwin':
            for path in test_framework:

                p = Path(path)
                if not p.absolute:
                    p = srctree() / p

                if (p / 'Headers/gl.h').exists():

                    self.prefix = p
                    self.cfg = Config()
                    self.cfg.add_system_include_path(self.prefix / 'Headers')
                    self.cfg.framework_add('Cocoa')
                    self.cfg.framework_add('OpenGL')

                    return

        raise Exception('unable to find gl.h in %s' % ', '.join(test + test_framework))

    def config(self):

        return self.cfg
