# -*- encoding: utf-8 -*-

import web
import json

from meta.page import Page

class Logout(Page):
    __template__ = 'logout.html'

    def GET(self):
        self.logout()
        return json.dumps({
            'success': True,
        })
        raise web.seeother('/')
