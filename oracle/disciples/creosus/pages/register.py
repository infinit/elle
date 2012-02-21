# -*- encoding: utf-8 -*-

import troll
from troll import form
import web

import pythia
import creosus

class Register(creosus.Page):
    __template__ = 'register.html'

    _form = form.Form('register_form', [
        form.Text('email', validator=form.validators.Email),
        form.Text('fullname', validator=form.validators.NotNull),
        form.Password('password', validator=form.validators.Regexp(r'^.{6,}$', 'Password too short')),
        form.Password('password_confirmation'),
        form.Text('activation_code'),
        form.Submit('register', label="Register", validator=form.validators.NotNull),
    ], validators=[
        form.validators.Validator(
            lambda f: f['password'] == f['password_confirmation'],
            "Passwords didn't match"
        ),
    ])

    @troll.view.exposeWhen('subscribe')
    def index(self):
        f = self._form.generate(self)
        if f.is_valid:
            res = self.api_admin.post('/register',{
                'email': f['email'].value,
                'fullname': f['fullname'].value,
                'password': f['password'].value,
            })
            if res['success']:
                raise web.seeother('/')
        return self.render(obj={
            'register_form': f.render(),
        })

