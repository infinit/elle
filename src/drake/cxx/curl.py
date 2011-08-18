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

class Curl(drake.Configuration):

    """Configuration for the Curl library."""

    def __init__(self, prefix = None):
        """Find and create a configuration for Boost.

        prefix -- Where to find curl, should contain
                  include/curl/curl.h.
        """
        # Compute the search path.
        if prefix is None:
            test = [Path('/usr'), Path('/usr/local')]
        else:
            test = [Path(prefix)]
        for i in range(len(test)):
            if not test[i].absolute:
                test[i] = srctree() / test[i]
        self.__prefix = self._search_all('include/curl/curl.h', test)[0]
        self.__config = drake.cxx.Config()
        self.__config.add_system_include_path(self.__prefix / 'include')
        self.__config.lib_path(self.__prefix / 'lib')

    def config(self):

        return self.__config

    def __repr__(self):
        return 'Curl(prefix = %s)' % repr(self.__prefix)
