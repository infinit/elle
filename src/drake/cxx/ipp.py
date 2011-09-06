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

class IPP(drake.cxx.LibraryConfiguration):

    """Configuration for the IPP library."""

    def __init__(self, prefix = None):
        """Find and create a configuration for IPP.

        prefix -- Where to find curl, should contain
                  include/ipp.h.
        """
        drake.cxx.LibraryConfiguration.__init__(self, prefix, 'include/ipp.h')
