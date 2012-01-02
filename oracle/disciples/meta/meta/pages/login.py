# -*- encoding: utf-8 -*-

from genshi import HTML
from web import form

from meta.session import users
from meta.page import Page

class Login(Page):
    __template__ = 'login.html'
    login_form = form.Form(
        form.Textbox("email", description="E-Mail"),
        form.Password("password", description="Password"),
        form.Button("submit", type="submit", description="Login"),
    )

    def GET(self):
        return self.render({
            'login_form': HTML(self.login_form.render()),
        })

    def POST(self):
        self.login_form.validates()
        user = users.authenticate(self.login_form.d.email, self.login_form.d.password)
        if user is not None:
            users.login(user)
            return 'bite'
        return self.render({
            'login_form': HTML(self.login_form.render()),
        })
