# -*- encoding: utf-8 -*-

import json

from meta.page import Page

class Root(Page):
    """
    Give basic server infos
        GET /
            -> {
                "server": "server name",
                "logged_in": True/False,
            }
    """

    __pattern__ = '/'

    def GET(self):
        return self.success({
            'server': 'Meta 0.1',
            'logged_in': self.user is not None,
        })

class Debug(Page):
    """
    This class is for debug purpose only
    """

    __pattern__ = "/debug"

    def POST(self):
        msg = self.data
        if self.notifier is not None:
            self.notifier.send_notify(msg)
        else:
            return self.error("notifier is not ready")
        return self.success({})
