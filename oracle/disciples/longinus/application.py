from __future__ import print_function

import sys

if "bsd" in sys.platform:
    from twisted.internet import kqreactor as _platform_reactor
elif "linux" in sys.platform:
    from twisted.internet import epollreactor as _platform_reactor
elif sys.platform.startswith('win'):
    from twisted.internet import iocpreactor as _platform_reactor
else:
    from twisted.internet import selectreactor as _platform_reactor

_platform_reactor.install()

from twisted.internet import reactor
from twisted.python import log

import longinus

class Application(object):
    def __init__(self, ip, port):
        self.ip = ip
        self.port = port
        pass

    def run(self):
        log.startLogging(sys.stderr)
        reactor.listenUDP(self.port, longinus.PunchHelper())
        reactor.run()
