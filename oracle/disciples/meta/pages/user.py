# -*- encoding: utf-8 -*-

import web

from meta.page import Page
from meta import conf, database
import metalib
import pythia
import json

class User(Page):
    """
    Get self infos
        GET /
            -> {
                'fullname': "My Name",
                'email': "My email",
                'devices': [device_id1, ...],
                'networks': [network_id1, ...]
                'identity': 'identity string',
                'public_key': 'public_key string',
                'accounts': [
                    {'type':'account type', 'id':'unique account identifier'}
                ]
            }

    Register a new user
        POST /register {
            'email': "email@pif.net", #required
            'fullname': "The full name", #required
            'password': "password', #required
            'admin_token': 'admin token', #required
        }

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

    GET /logout
        -> {
            'success': True
        }
    """

    def GET(self, action=None):
        if not action:
            return self._me()
        elif action == 'logout':
            return self._logout()
        raise Exception("Unknown action: %s" % str(action))

    def POST(self, action):
        if action == 'register':
            return self._register()
        elif action == 'login':
            return self._login()
        raise Exception("Unknown action: %s" % str(action))

    def _logout(self):
        if not self.user:
            return self.error("Not logged in")
        self.logout()
        return self.success()

    def _me(self):
        if not self.user:
            return self.error("Not logged in")
        return self.success({
            '_id': self.user['_id'],
            'fullname': self.user['fullname'],
            'email': self.user['email'],
            'devices': self.user.get('devices', []),
            'networks': self.user.get('networks', []),
            'identity': self.user['identity'],
            'public_key': self.user['public_key'],
            'accounts': self.user['accounts'],
        })

    def _login(self):
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

    _validators = {
        'email': web.form.regexp(r".*@.*", "must be a valid email address"),
        'fullname': web.form.regexp(r".{3,90}$", 'fullname must be between 3 and 90 characters'),
        'password': web.form.regexp(r".{64,64}$", 'password must be between 3 and 20 characters'), #XXX min password size
    }

    def _register(self):
        if self.user is not None:
            return self.error("Please logout before any register attempt")

        user = self.data

        errors = []
        for k, v in self._validators.items():
            if not k in user:
                errors.append("Field '%s' is mandatory" % k)
            else:
                f = unicode(user[k]).strip()
                if not v.valid(f):
                    errors.append(v.msg)
                else:
                    user[k] = f

        if not len(errors) and database.users.find_one({'email': user['email']}):
            errors.append('This email is already registered')
        if len(errors):
            return json.dumps({
                'success': False,
                'errors': errors,
                'error': ', '.join(errors),
            })

        user["_id"] = str(database.users.save({}))

        identity, public_key = metalib.generate_identity(
            user["_id"],
            user['email'], user['password'],
            conf.INFINIT_AUTHORITY_PATH,
            conf.INFINIT_AUTHORITY_PASSWORD
        )

        user_id = self.registerUser(
            _id=user["_id"],
            email=user['email'],
            fullname=user['fullname'],
            password=self.hashPassword(user['password']),
            identity=identity,
            public_key=public_key,
            networks=[],
            devices=[],
            accounts=[
                {'type':'email', 'id': user['email']}
            ]
        )

        network = {
            'name': "private",
            'model': 'slug',
            'owner': user_id,
            'users': [],
            'devices': [],
            'descriptor': None,
            'root_block': None,
            'root_address': None,
        }
        network_id = database.networks.save(network);
        user = database.users.find_one({'_id': user_id})
        user['networks'].append(network_id)
        user = database.users.save(user)
        return self.success()
