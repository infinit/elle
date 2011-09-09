# Copyright (C) 2009-2011, Quentin "mefyl" Hocquet
#
# This software is provided "as is" without warranty of any kind,
# either expressed or implied, including but not limited to the
# implied warranties of fitness for a particular purpose.
#
# See the LICENSE file for more information.

import drake
import drake.cxx
import sys

from .. import Exception, Path, Version, srctree
from .  import Config, StaticLib

class Boost(drake.Configuration):

    """Configuration for the Boost C++ library collection"""

    def __init__(self, cxx_toolkit = drake.cxx.Toolkit(), prefix = None, version = Version()):
        """Find and create a configuration for Boost.

        prefix -- Where to find Boost, should contain
                  include/boost/version.hpp among others. /usr and
                  /usr/local are searched by default. If relative, it
                  is rooted in the source tree.
        version -- Requested version.
        """
        # Compute the search path.
        if prefix is None:
            test = [Path('/usr'), Path('/usr/local')]
        else:
            test = [Path(prefix)]
        for i in range(len(test)):
            if not test[i].absolute():
                test[i] = srctree() / test[i]
        test = self._search_all('include/boost/version.hpp', test)
        miss = []
        # Try every search path
        for path in test:
            # Create basic configuration for version checking.
            cfg = Config()
            cfg.add_system_include_path('%s/include' % path)
            cfg.lib_path('%s/lib' % path)
            # Check the version.
            version_eff = cxx_toolkit.preprocess(b'#include <boost/version.hpp>\nBOOST_VERSION',
                                             config = cfg)
            version_eff = int(version_eff.split('\n')[-2].strip())
            version_eff = Version(version_eff // 100000, version_eff // 100 % 1000, version_eff % 100)
            if version_eff not in version:
                miss.append(version_eff)
                continue
            # Fill configuration
            self.__prefix = path
            self.cfg = cfg
            self.cfg_test = Config()
            self.cfg_test.lib('boost_unit_test_framework')
            self.cfg_thread = Config()
            self.cfg_thread.lib('boost_thread')
            self.cfg_system = Config()
            self.cfg_system.lib('boost_system')
            self.cfg_filesystem = Config()
            self.cfg_filesystem.lib('boost_filesystem')
            self.cfg_signals = Config()
            self.cfg_signals.lib('boost_signals')
            self.cfg_date = Config()
            self.cfg_date.lib('boost_date_time')
            self.cfg_regex = Config()
            self.cfg_regex.lib('boost_regex')
            return

        raise Exception('no matching boost for the requested version (%s) in %s. Found versions: %s.' % \
                            (version, self._format_search(test), ', '.join(map(str, miss))))

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

    def config_signals(self):

        return self.cfg_signals

    def config_date(self):

        return self.cfg_date

    def config_regex(self):

        return self.cfg_regex

    def __repr__(self):
        return 'Boost(prefix = %s)' % repr(self.__prefix)
