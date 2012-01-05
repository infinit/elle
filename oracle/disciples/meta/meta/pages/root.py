# -*- encoding: utf-8 -*-

import json

from meta.page import Page

class Root(Page):

    def GET(self):
        return json.dumps({
            'server': 'Meta 0.1',
            'logged_in': self.user is not None,
        })
