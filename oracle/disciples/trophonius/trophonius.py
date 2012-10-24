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

class InvalidID(Exception):

	def __init__(self, id):
		self.id = id
		pass

	def __str__(self):
		return "{} invalid id".format(self.id)

class Trophonius(basic.LineReceiver):

	states = ('HELLO',
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
		log.msg("Connection lost with", self.transport.getPeer(), reason)
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
		hdl = getattr(self, "handle_{}".format(self.state), None)
		if hdl is not None:
			hdl(line)

class MetaTropho(basic.LineReceiver):

	delimiter = "\n"

	def __init__(self, factory):
		self.factory = factory

	def connectionMade(self):
		log.msg("New connection from", self.transport.getPeer())

	def connectionLost(self, reason):
		log.msg("Connection lost with", self.transport.getPeer(), reason)

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

	def enqueue(self, line, recipients):
		try:
			for rec_id in recipients:
				for c in self.factory.clients[rec_id]:
					c.sendLine(line)
		except KeyError as ke:
			log.err("Handled exception {}: {} unknow id".format(
				ke.__class__.__name__,
				ke))
			raise InvalidID(rec_id)

	def make_switch(self, line):
		try:
			recipients_ids = ""
			js_req = json.loads(line)
			_recipient = js_req["recipient_id"]
			if isinstance(_recipient, list):
				recipients_ids = _recipient
			elif isinstance(_recipient, str) or isinstance(_recipient, unicode):
				recipients_ids = [_recipient]
			self.enqueue(line, recipients_ids)
		except ValueError as ve:
			log.err("Handled exception {}: {}".format(
				ve.__class__.__name__,
				ve))
			self._send_res(res=400, msg=ve.message)
		except KeyError as ke:
			log.err("Handled exception {}: missing key {} in request".format(
				ke.__class__.__name__,
				ke))
			self._send_res(res=400, msg="missing key {} in req".format(ke.message))
		except InvalidID as ide:
			self._send_res(res=400, msg="{}".format(str(ide)))
		else:
			self._send_res(res=202, msg="message enqueued")

	def lineReceived(self, line):
		self.make_switch(line)

class TrophoFactory(protocol.Factory):
	def __init__(self, application):
		self.clients = application.clients

	def startFactory(self):
		pass

	def buildProtocol(self, addr):
		return Trophonius(self)

class MetaTrophoFactory(protocol.Factory):
	def __init__(self, app):
		self.clients = app.clients
		pass

	def buildProtocol(self, addr):
		return MetaTropho(self)

