# -*- encoding: utf-8 -*-

import web
import json

from meta.page import Page

class Logout(Page):
    __template__ = 'logout.html'

    def GET(self):
        self.logout()
        if self.input.get('fmt') == 'json':
            return json.dumps({
                'success': True,
            })
        raise web.seeother('/')
