import json
import web

from meta.page import Page
from meta import database
from meta import error
from meta import regexp

import meta.mail

import re

import metalib


class Get(Page):
    """
    POST {
              'count': the number of notif you want to pull.
              'offset': the offset.
         }
        -> {
                'notifs': [{}, {}, {}]
                'old_notifs': [{}, {}]
           }
    """

    __pattern__ = "/notification/get"

    def POST(self):
        if not self.user:
            return self.error(error.NOT_LOGGED_IN)

        _user = database.users().find_one(database.ObjectId(self.user['_id']))

        if not _user:
          return self.error(error.UNKNOWN_USER)

        #XXX: limit
        try:
            limit = self.data['limit']
        except Exception as e:
            pass

        try:
            count = self.data['count']
            offset = self.data['offset']
        except Exception as e:
            count = limit
            offset = 0

        notifs = _user['notifications'][offset:]
        old_notifs = []
        if (len(notifs) < count):
            old_notifs = _user['old_notifications'][:(count - len(notifs))]

        return self.success({
            'notifs' : notifs,
            'old_notifs': old_notifs,
        })

class Read(Page):
    """
    POST {

         }
         -> {
            }

    """

    __pattern__ = "/notification/read"

    def GET(self):
        if not self.user:
            return self.error(error.NOT_LOGGED_IN)

        self._user['old_notifications'] = self._user['notifications'] + self._user['old_notifications']

        self._user['notifications'] = []

        database.users().save(self._user)

        return self.success()
