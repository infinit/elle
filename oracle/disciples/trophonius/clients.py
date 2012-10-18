#!/usr/bin/env python2
# -*- encoding: utf-8 -*-

from __future__ import print_function

import Queue

class ClientList(object):
	def __init__(self):
		self.clients = {}
		pass

	def add(self, client):
		if client.id not in self.clients:
			self.clients[client.id] = []
		self.clients[client.id].append(client)
		pass

	def remove(self, client):
		self.clients.pop(client.id)
		pass

	def logged_in(self):
		return self.clients.viewvalues()

	def unique(self):
		s = set()
		for cs in self.clients.viewvalues():
			for c in cs:
				s.add(c.id)
		return s.__iter__()


	def __iter__(self):
		def __iter___(me):
			for cs in self.clients.viewvalues():
				for c in cs:
					yield c
		return __iter___(self)

	def __contains__(self, item):
		for cs in self.clients.viewvalues():
			for c in cs:
				if c is item:
					return True
		return False

	def __getitem__(self, key):
		return self.clients[key]

	def __str__(self):
		return str(self.clients)
