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
                'token': "generated session token",
                'fullname': 'full name',
                'identity': 'Full base64 identity',
            }
    """

    def POST(self):
        if self.user is not None:
            return self.error("Already logged in")
        email = self.data.get('email')
        password = self.data.get('password')
        if self.authenticate(email, password):
            return self.success({
                'token': self.session.session_id,
                'fullname': self.user['fullname'],
                'email': self.user['email'],
                'identity': self.user['identity'],
            })
        return self.error('Wrong login/password')
