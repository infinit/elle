# -*- encoding: utf-8 -*-

import socket
import json

class Notifier(object):

	def open(self) : pass

	def send_notify(self, to, message): pass

	def close(self) : pass


class TrophoniusNotify(Notifier):

	def open(self):
		self.conn = socket.socket()
		self.conn.connect(("localhost", 23457))

	def send_notify(self, message):
		if isinstance(message, dict):
			msg = json.dumps(dict)
		else isinstance(message, str):
			msg = message
		self.conn.send(msg)
