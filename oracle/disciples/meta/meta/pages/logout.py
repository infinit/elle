# -*- encoding: utf-8 -*-

import web
import json

from meta.page import Page

class Logout(Page):
    """
    GET /
        -> {
            'success': True
        }
    """

    def GET(self):
        if not self.user:
            return self.error("Not logged in")
        self.logout()
        return json.dumps({
            'success': True,
        })
        raise web.seeother('/')
