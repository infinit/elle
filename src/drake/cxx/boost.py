from .. import Path, Exception, srctree
from .  import Config, StaticLib

class Boost:

    def __init__(self, prefix = None, major = None, minor_min = None):

        if prefix is None:
            test = ['/usr', '/usr/local']
        else:
            test = [prefix]

        for path in test:
            p = Path(path)
            if not p.absolute:
                p = srctree() / p
            if (p / 'include/boost/version.hpp').exists():

                self.prefix = path

                self.cfg = Config()
                self.cfg.add_system_include_path('%s/include' % self.prefix)

                return

        raise Exception('unable to find boost/version.hpp in %s' % ', '.join(test))

    def config(self):

        return self.cfg
