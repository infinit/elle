# -*- encoding: utf-8 -*-

import json

from meta.page import Page

class Root(Page):
    __template__ = 'root.html'

    def GET(self):
        if self.input.get('fmt') == 'json':
            return json.dumps({
                'server': 'Meta 0.1',
                'token': self.session.session_id,
                'logged_in': self.user is not None,
            })
        return self.render()
