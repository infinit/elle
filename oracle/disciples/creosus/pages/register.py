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
        # XXX
        #form.Password('password', validator=form.validators.Regexp(r'^.{6,}$', 'Password too short')),
        #form.Password('password_confirmation'),
        form.Password('activation_code', label="Activation code"),
        form.Submit('register', label="Register", validator=form.validators.NotNull),
    ], validators=[
        # XXX
        #form.validators.Validator(
        #    lambda f: f['password'] == f['password_confirmation'],
        #    "Passwords didn't match"
        #),
    ])

    @troll.view.exposeWhen('subscribe')
    def index(self):
        form = self._form.generate(self)
        if form.is_valid:
            res = self.api_admin.post('/register',{
                'email': form['email'].value,
                'fullname': form['fullname'].value,
                'password': form['password'].value,
                'activation_code': form['activation_code'].value,
            })
            if res['success']:
                raise web.seeother('/')
            elif 'errors' in res:
                form.errors.extend(res['errors'])
            elif 'error' in res:
                form.errors.append(res['error'])
            else:
                form.errors.append("Unknown API error")
        return self.render(obj={
            'register_form': form.render(),
        })

