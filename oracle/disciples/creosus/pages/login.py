# -*- encoding: utf-8 -*-

import troll
from troll import form
from troll.form import validators

import creosus

class Login(creosus.Page):
    __template__ = 'login.html'

    _form = form.Form(
        'login_form',
        [
            form.Text('email', validator=validators.Email),
            form.Password('password', validator=validators.NotNull),
            form.Submit('submit', label="Log in"),
        ]
    )

    @troll.view.expose
    def index(self):
        f = self._form.generate(self)
        if f.is_valid:
            res = self.api.post('/login', {
                'email': f['email'].value,
                'password': f['password'].value,
            })
            if res['success']:
                self.api_session['token'] = res['token']
                raise web.seeother('/')
            else:
                f.errors.append(res['error'])
            print res
        return self.render(obj={'login_form': f.render()})

