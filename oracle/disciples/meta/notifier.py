# -*- encoding: utf-8 -*-

import socket
import json

import meta.page
import database

from twisted.python import log

FILE_TRANSFER = 7
FILE_TRANSFER_STATUS = 11
USER_STATUS = 8
MESSAGE = 217

class Notifier(object):

        def open(self) : pass

	def notify_one(self, notif_id, to, message): pass

	def notify_some(self, notif_id, to, message): pass

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

	def notify_one(self, notification_id, recipient_id, message):
                _user = database.users().find_one(
                            database.ObjectId(recipient_id));

                if not _user:
                        return False

                message['notification_id'] = notification_id;

                _user['notifications'].append(message)
                database.users().save(_user)

                print("is that mother fucking user connected ?", _user['connected'])
                if _user['connected']:
                        _dict = {'recipient_id': str(recipient_id)}
                        _dict.update(message)
                        print(_dict)
                        self.send_notification(_dict)

                return True

        def send_notify_some(self, notification_id, recipients_id, message):
                if not isinstance(recipients_id, list):
                        return self.notify_one(notification_id, recipients_id, message)
                ret = True
                for _id in recipients_id:
                        ret = ret and self.notify_one(notification_id, _id, message)
                return ret
