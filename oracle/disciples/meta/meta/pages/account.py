# -*- encoding: utf-8 -*-

import json

from meta.page import Page

class Account(Page):

    def GET(self):
        return json.dumps({
            'token': self.session.session_id,
            'fullname': self.user['fullname'],
            'email': self.user['email'],
        })

