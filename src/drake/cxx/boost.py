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

                self.cfg_test = Config()
                self.cfg_test.lib('boost_unit_test_framework')

                self.cfg_thread = Config()
                self.cfg_thread.lib('boost_thread')

                self.cfg_system = Config()
                self.cfg_system.lib('boost_system')

                self.cfg_filesystem = Config()
                self.cfg_filesystem.lib('boost_filesystem')

                return

        raise Exception('unable to find boost/version.hpp in %s' % ', '.join(test))

    def config(self):

        return self.cfg

    def config_test(self):

        return self.cfg_test

    def config_thread(self):

        return self.cfg_thread

    def config_system(self):

        return self.cfg_system

    def config_filesystem(self):

        return self.cfg_filesystem
