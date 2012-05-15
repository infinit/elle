# -*- encoding: utf-8 -*-

import web

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
            form.Password('password'), #XXX, validator=validators.NotNull),
            form.Submit('submit', label="Log in"),
        ]
    )

    @troll.view.exposeWhen('login')
    def index(self):
        f = self._form.generate(self)
        if f.is_valid:
            res = self.api.post('/login', {
                'email': f['email'].value,
                'password': f['password'].value,
            })
            if res['success']:
                s = self.app.session_store.generateNewSession(
                    self.app.conf['salt'],
                    troll.security.db.User({
                        'id': True, # auth test are based on the test 'id is not None'
                        'mail': res['email'],
                        'fullname': res['fullname'],
                        'role_id': 'user',
                    })
                )
                self.app.session_hash = s.hash
                s['api_session'] = {
                    'token': res['token'],
                }
                raise web.seeother('/')
            else:
                f.errors.append(res['error'])
        if f['email'].value is not None:
            f['password'].value = ''
        else:
            f['password'].value = None
        return self.render(obj={'login_form': f.render()})

