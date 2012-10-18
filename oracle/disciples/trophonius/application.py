#!/usr/bin/env python2
# -*- encoding: utf-8 -*-

from __future__ import print_function

import sys

if "bsd" in sys.platform:
	from twisted.internet import kqreactor as _platform_reactor
elif "linux" in sys.platform:
	from twisted.internet import epollreactor as _platform_reactor
elif "win" in sys.platform:
	from twisted.internet import iocpreactor as _platform_reactor
else:
	from twisted.internet import selectreactor as _platform_reactor

_platform_reactor.install()

from twisted.internet import reactor, protocol, inotify
from twisted.python import filepath
from twisted.python import log

import trophonius
import os

try:
	import setproctitle
	HAVE_SETPROCTITLE = True
except:
	HAVE_SETPROCTITLE = False
	pass

class Message(object):
	def __init__(self, type, **kwars):
		self.type = type

class Application(object):
	def __init__(self, ip="127.0.0.1", port=1245, logfile=sys.stderr):
		self.ip = ip
		self.port = port
		self.logfile = logfile
		if HAVE_SETPROCTITLE:
			setproctitle.setproctitle("Trophonius")

	def run(self):
		notifier = inotify.INotify()
		notifier.startReading()
		notifier.watch(filepath.FilePath("./trophonius/"), callbacks=[mod_reload])
		log.startLogging(self.logfile)
		reactor.listenTCP(self.port, trophonius.TrophoFactory(self))
		reactor.run()

def mod_reload(self, filepath, mask):
	human_masks = inotify.humanReadableMask(mask)
	if filepath.path.endswith(".py") and "modify" in human_masks:
		mod_name, ext = os.path.splitext(os.path.basename(filepath.path))
		if not __name__.endswith(mod_name):
			log.msg("Reloading {}".format(mod_name))
			reload(globals()[mod_name])


