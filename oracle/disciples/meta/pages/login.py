# -*- encoding: utf-8 -*-

import json
import web

from meta.page import Page

class Login(Page):
    """
    Generate a token for further communication
        POST /login {
            "email": "mail !",
            "password": "password",
        }
            -> {
                'success': True,
                'token': "generated session token"
            }
    """

    def POST(self):
        if self.user is not None:
            return self.error("Already logged in")
        email = self.data.get('email')
        password = self.data.get('password')
        if self.authenticate(email, password):
            return json.dumps({
                'success': True,
                'token': self.session.session_id,
            })
        return json.dumps({
            'success': False,
            'error': 'Wrong login/password',
        })
