# -*- encoding: utf-8 -*-

from web import form
from genshi import HTML

from meta.page import Page
from meta.session import users

class Register(Page):
    vpass = form.regexp(r".{3,20}$", 'must be between 3 and 20 characters')
    vfullname = form.regexp(r".{3,90}$", 'must be between 3 and 90 characters')
    vemail = form.regexp(r".*@.*", "must be a valid email address")

    register_form = form.Form(
        form.Textbox("fullname", vfullname, description="Full name"),
        form.Textbox("email", vemail, description="E-Mail"),
        form.Password("password", vpass, description="Password"),
        form.Password("password2", description="Repeat password"),
        form.Button("submit", type="submit", description="Register"),
        validators = [
            form.Validator("Passwords did't match", lambda i: i.password == i.password2)
        ]
    )
    __template__ = 'register.html'

    def GET(self):
        return self.render(obj={
            'register_form': HTML(self.register_form.render())
        })

    def POST(self):
        if not self.register_form.validates():
            return self.render({
            'register_form': HTML(self.register_form.render())
        })
        users.register(
            email=self.register_form.d.email,
            fullname=self.register_form.d.fullname,
            password=users.pswd(self.register_form.d.password),
        )
        return self.render(template='register_success.html')

