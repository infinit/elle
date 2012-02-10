# -*- encoding: utf-8 -*-

import troll
import web

import creosus

class Register(creosus.Page):
    __template__ = 'register.html'

    _form = troll.form.Form('register_form', [
        troll.form.Text('email', validator=troll.form.validators.Email),
        troll.form.Text('fullname', validator=troll.form.validators.NotNull),
        troll.form.Password('password', validator=troll.form.validators.Regexp(r'^.{6,}$', 'Password too short')),
        troll.form.Password('password_confirmation'),
        troll.form.Text('activation_code'),
        troll.form.Hidden('csrf'),
        troll.form.Submit('register', label="Register"),
    ])

    @troll.view.expose
    def index(self):
        f = self._form.generate()
        if f['csrf'].value or True:
            print f, 'IS VALID',  f.is_valid


        return self.render(obj={
            'register_form': f.render(),
        })

