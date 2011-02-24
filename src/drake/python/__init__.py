# Copyright (C) 2009-2011, Quentin "mefyl" Hocquet
#
# This software is provided "as is" without warranty of any kind,
# either expressed or implied, including but not limited to the
# implied warranties of fitness for a particular purpose.
#
# See the LICENSE file for more information.

import commands, platform
from .. import Path, Exception, srctree

def check_version(expected, effective):

    try:
        iter(expected)
        return effective in expected
    except TypeError:
        return effective == expected

class Python:

    def __init__(self, major, minor = None, modules = [], prefix = None):

        if prefix is None:
            test = ['/usr', '/usr/local']
        else:
            test = [prefix]


        self.prefix = None
        for path in test:
            p = Path(path)
            if not p.absolute:
                p = srctree() / p
            if (p / 'bin/python').exists():
                self.prefix = p
                break

        if self.prefix is None:
            raise Exception('unable to find bin/python in %s' % ', '.join(test))

        self.python = self.prefix / 'bin/python'

        self.major = eval(commands.getoutput('%s -c \"import sys\nprint sys.version_info[0]\"' % self.python))
        self.minor = eval(commands.getoutput('%s -c \"import sys\nprint sys.version_info[1]\"' % self.python))

        if not check_version(major, self.major):
            raise Exception('invalid python major version: expected %s.x, got %s.%s' % (major, self.major, self.minor))
        if not check_version(minor, self.minor):
            raise Exception('invalid python minor version: expected %s.%s, got %s.%s' % (major, minor, self.major, self.minor))

        linux_command = {
          'Ubuntu': 'apt-get install %s'
          }


        linux_pck = {
          'xml.dom.ext': {
            'Ubuntu': 'python-xml'
            }
          }

        for module in modules:
            if commands.getstatusoutput('%s -c \'import %s\'' % (self.python, module))[0] >> 8 != 0:
                msg = 'missing python module %s' % module
                if platform.system() == 'Linux':
                    distro = platform.linux_distribution()[0]
                    if module in linux_pck \
                          and distro in linux_pck[module]:
                        pck = linux_pck[module][distro]
                        cmd = linux_command[distro]
                        msg = '%s - try `%s\'' % (msg,cmd % pck)
                raise Exception(msg)
