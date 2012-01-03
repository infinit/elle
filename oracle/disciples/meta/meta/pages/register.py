# -*- encoding: utf-8 -*-

import json
from web import form
from genshi import HTML

from meta import database
from meta.page import Page

class Register(Page):

    __template__ = 'register.html'

    def GET(self):
        return self.render(obj={
            'register_form': HTML(self.register_form.render())
        })

    def POST(self):
        f = self.register_form
        if not f.validates():
            if self.input.get('fmt') == 'json':
                errors = [
                    i.note for i in filter(f.inputs, lambda i: i.note is not None)
                ]
                return json.dumps({
                    'success': False,
                    'errors': errors,
                })
            return self.render({
                'register_form': HTML(f.render())
            })
        self.registerUser(
            email=f.d.email,
            fullname=f.d.fullname,
            password=self.hashPassword(f.d.password),
        )
        if self.input.get('fmt') == 'json':
            return json.dumps({
                'success': True,
            })
        return self.render(template='register_success.html')

    class _NotAlreadyRegistered:
        msg = "Already registered user"
        @staticmethod
        def valid(value):
            return not database.users.find_one({'email': value})

    _vpass = form.regexp(r".{3,20}$", 'must be between 3 and 20 characters')
    _vfullname = form.regexp(r".{3,90}$", 'must be between 3 and 90 characters')
    _vemail = form.regexp(r".*@.*", "must be a valid email address")

    @property
    def register_form(self):
        if self.input.get('fmt') == 'json':
            return form.Form(
                form.Textbox("fullname", self._vfullname),
                form.Textbox("email", self._vemail, self._NotAlreadyRegistered),
                form.Password("password", self._vpass),
            )
        return form.Form(
            form.Textbox("fullname", self._vfullname, description="Full name"),
            form.Textbox("email", self._vemail, self._NotAlreadyRegistered, description="E-Mail"),
            form.Password("password", self._vpass, description="Password"),
            form.Password("password2", description="Repeat password"),
            form.Button("submit", type="submit", description="Register"),
            validators = [
                form.Validator("Passwords did't match", lambda i: i.password == i.password2)
            ]
        )
