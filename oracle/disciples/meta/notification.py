# -*- encoding: utf-8 -*-

import socket
import json

import meta.page
import database

from twisted.python import log

class Notifier(object):

        def open(self) : pass

	def notify_one(self, to, message): pass

	def notify_some(self, to, message): pass

        def send_notification(self, message):
                raise Exception('Not implemented')

	def close(self) : pass


class TrophoniusNotify(Notifier):
	def __init__(self):
		self.conn = socket.socket()

	def open(self):
		self.conn.connect(("localhost", 23457))

        def send_notification(self, message):
                if isinstance(message, dict):
			msg = json.dumps(message, default = str)
		# elif isinstance(message, str):
		# 	msg = message + json.dumps(_dict, default = str)
                else:
                        log.err('Notification was bad formed.')

		self.conn.send("{}\n".format(msg))

	def notify_one(self, recipient_id, message):
                _user = database.users().find_one(
                            database.ObjectId(recipient_id));

                print(_user['notifications'])

                _user['notifications'].append(message)
                database.users().save(_user)

                if _user['connected']:
                        _dict = {'recipient_id': recipient_id}
                        _dict.update(message)
                        self.send_notification(_dict)

        def send_notify_some(self, recipients_id, message):
                if not isinstance(recipients_id, list):
                        self.notify_one(recipients_id, message)
                for _id in recipients_id:
                        self.notify_one(_id, message)
