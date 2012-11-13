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
              'limit': the number of notif you want to pull.
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

        limit = self.data['limit']

        notifs = _user['notifications']
        old_notifs = []
        if (len(notifs) < limit):
            old_notifs = _user['old_notifications'][:(limit - len(notifs))]

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
