#!/usr/bin/env python2
# -*- encoding: utf-8 -*-

from __future__ import print_function

from twisted.internet import protocol, reactor
from twisted.protocols import basic
from twisted.python import log

import json
import time
import pythia
import pprint
import clients

response_matrix = {
	# Success code
	200 : "OK",
	202 : "accepted",

	# Client Error code
	400 : "bad_request",
	403 : "forbidden",
	404 : "not_found",
	418 : "im_a_teapot",

	#  Server error
	500 : "internal_server" ,
	666 : "unknown_error" 
}

class Trophonius(basic.LineReceiver):

	states = ('HELLO',
			  'NOTIF',
			  'CHAT')

	delimiter = "\n"
	def __init__(self, factory):
		self.factory = factory
		self.state = 'HELLO'
	
	def __str__(self):
		if hasattr(self, "id"):
			return "<{}({})>".format(self.__class__.__name__,
					"id={}".format(self.id))
		return "<{}()>".format(self.__class__.__name__)

	def connectionMade(self):
		log.msg("New connection from", self.transport.getPeer())

	def connectionLost(self, reason):
		log.msg("Connection lost with", self.transport.getPeer())
		if self in self.factory.clients:
			self.factory.clients.remove(self)

	def _send_res(self, res, msg=""):
		if isinstance(res, dict):
			self.sendLine(json.dumps(res))
		elif isinstance(res, int):
			s = {}
			s["response_code"] = res
			if msg:
				s["response_details"] = "{}: {}".format(response_matrix[res], msg)
			else:
				s["response_details"] = "{}".format(response_matrix[res])
			message = json.dumps(s)
			self.sendLine(message)

	def handle_CHAT(self, line):
		"""
		Just a dummy function
		"""
		for c in (_c for _c in self.factory.clients if _c is not self):
			log.msg("sending {} to <{}>".format(line, c.transport.getPeer()))
			c.sendLine("{}".format(line))
		self._send_res(200)

	def enqueue(self, message, recipients_ids):
		pass

	def handle_NOTIF(self, line):
		"""
		This function handle the second part of the protocol, after the client
		has authenticate.
		"""
		try:
			js_req = json.loads(line)
			_recipient = js_req["recipient_id"]
			if isinstance(_recipient, list):
				recipients_ids = _recipient
			elif isinstance(_recipient, str):
				recipients_ids = [_recipient]
		except ValueError as ve:
			log.err("Handled exception {} in state {}: {}".format(
				ve.__class__.__name__,
				self.state,
				ve))
			self._send_res(res=400, msg=ve.message)
			self.transport.loseConnection()
		else:
			self._send_res(res=202, msg="message enqueued")
		self.enqueue(line, recipients_ids)

	def handle_HELLO(self, line):
		"""
		This function handle the first message of the client
		It waits for a json object with:
		{
			"token" : <token>
		}
		"""
		try:
			js_req = json.loads(line)
			cl = pythia.Client(session={"token": js_req["token"]})
			res = cl.get('/self')
			# The authentication succeeded

			self.id = res["_id"]
			self.token = js_req["token"]
			# Add the current client to the client list
			self.factory.clients.add(self)

			# Enable the notifications for the current client
			self.state = "CHAT"
			print("Switching state to", self.state)

			# Send the success to the client
			self._send_res(res=200)
		except (ValueError, KeyError) as ve:
			log.err("Handled exception {} in state {}: {}".format(
				ve.__class__.__name__,
				self.state,
				ve))
			self._send_res(res=400, msg=ve.message)
			self.transport.loseConnection()

	def lineReceived(self, line):
		print(line)
		hdl = getattr(self, "handle_{}".format(self.state), None)
		if hdl is not None:
			hdl(line)

class TrophoFactory(protocol.Factory):
	def __init__(self, application):
		self.clients = clients.ClientList()
		self.queue = {}

	def dequeue(self):
		pass

	def startFactory(self):
		reactor.callFromThread(self.dequeue)
		pass

	def buildProtocol(self, addr):
		return Trophonius(self)

