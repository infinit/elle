# -*- encoding: utf-8 -*-

from genshi import HTML
from web import form
import json

from meta.page import Page

class Login(Page):
    __template__ = 'login.html'
    def GET(self):
        return self.render({
            'login_form': HTML(self.login_form.render()),
        })

    def POST(self):
        if self.input.get('fmt') == 'json':
            email = self.input.get('email')
            password = self.input.get('password')
            if self.authenticate(email, password):
                return json.dumps({
                    'success': True,
                    'token': self.session.session_id,
                })
            else:
                return json.dumps({
                    'success': False,
                    'error': 'Wrong login/password',
                })
        self.login_form.validates()
        if self.authenticate(self.login_form.d.email, self.login_form.d.password):
            raise web.seeother('/')
        return self.render({
            'login_form': HTML(self.login_form.render()),
        })

    @property
    def login_form(self):
        return form.Form(
            form.Textbox("email", description="E-Mail"),
            form.Password("password", description="Password"),
            form.Button("submit", type="submit", description="Login"),
        )

