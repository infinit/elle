#!/usr/bin/env python2
# -*- encoding: utf-8 -*-

from __future__ import print_function

import sys
from constants import *

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

from OpenSSL import crypto

import trophonius
import clients
import os
import os.path

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
	def __init__(self, ip="127.0.0.1", port=TROPHONIUS_LISTEN_TCP_PORT, logfile=sys.stderr, ssl_port=TROPHONIUS_LISTEN_SSL_PORT):
		self.ip = ip
		self.port = port
		self.logfile = logfile
		self.ssl_port = ssl_port
		self.clients = clients.ClientList()
		if HAVE_SETPROCTITLE:
			setproctitle.setproctitle("Trophonius")

	def create_self_signed_cert(self, cert_dir):
		"""
		If datacard.crt and datacard.key don't exist in cert_dir, create a new
		self-signed cert and keypair and write them into that directory.
		"""
		# create a key pair
		k = crypto.PKey()
		k.generate_key(crypto.TYPE_RSA, 2048)

		# create a self-signed cert
		cert = crypto.X509()
		cert.get_subject().C = "FR"
		cert.get_subject().ST = "France"
		cert.get_subject().L = "Paris"
		cert.get_subject().O = "Infinit"
		cert.get_subject().OU = "Infinit.io"
		cert.get_subject().CN = "Ta mere"
		cert.set_serial_number(1000)
		cert.gmtime_adj_notBefore(0)
		cert.gmtime_adj_notAfter(10*365*24*60*60)
		cert.set_issuer(cert.get_subject())
		cert.set_pubkey(k)
		cert.sign(k, 'sha256')

		self.ssl_key_path = os.path.join(cert_dir, TROPHONIUS_SSL_KEY)
		self.ssl_cert_path = os.path.join(cert_dir, TROPHONIUS_SSL_CERT)
		open(self.ssl_cert_path, "wt").write(
				crypto.dump_certificate(crypto.FILETYPE_PEM, cert))
		open(self.ssl_key_path, "wt").write(
				crypto.dump_privatekey(crypto.FILETYPE_PEM, k))

	def run(self):
		if not all(os.path.exists(file) for file in (TROPHONIUS_SSL_KEY, TROPHONIUS_SSL_CERT)):
			self.create_self_signed_cert(".")
		notifier = inotify.INotify()
		notifier.startReading()
		notifier.watch(filepath.FilePath("./trophonius/"), callbacks=[mod_reload])
		log.startLogging(self.logfile)
		factory = trophonius.TrophoFactory(self)
		meta_factory = trophonius.MetaTrophoFactory(self)
		reactor.listenTCP(self.port, factory)
		reactor.listenTCP(self.ssl_port, meta_factory)
		reactor.run()

def mod_reload(self, filepath, mask):
	human_masks = inotify.humanReadableMask(mask)
	if filepath.path.endswith(".py") and "modify" in human_masks:
		mod_name, ext = os.path.splitext(os.path.basename(filepath.path))
		if not __name__.endswith(mod_name):
			log.msg("Reloading {}".format(mod_name))
			reload(globals()[mod_name])


