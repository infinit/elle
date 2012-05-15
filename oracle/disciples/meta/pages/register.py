# -*- encoding: utf-8 -*-

import json
import web.form
import time

import pythia
import metalib

from meta import conf, database
from meta.page import Page

class Register(Page):
    """
    Register a new user
    POST /register {
        'email': "email@pif.net", #required
        'fullname': "The full name", #required
        'password': "password', #required
        'admin_token': 'admin token', #required
    }

    """

    _validators = {
        'email': web.form.regexp(r".*@.*", "must be a valid email address"),
        'fullname': web.form.regexp(r".{3,90}$", 'fullname must be between 3 and 90 characters'),
        'password': web.form.regexp(r".{0,20}$", 'password must be between 3 and 20 characters'), #XXX min password size
    }

    def POST(self):
        if self.user is not None:
            return self.error("Please logout before any register attempt")

        user = self.data

        if user.get('admin_token') != pythia.constants.ADMIN_TOKEN:
            return self.error("You are not allowed to do that")

        errors = []
        for k, v in Register._validators.items():
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
            })

        identity, identity_pub = metalib.generate_identity(
            user['email'], user['password'],
            conf.INFINIT_AUTHORITY_PATH,
            conf.INFINIT_AUTHORITY_PASSWORD
        )

        user_id = self.registerUser(
            email=user['email'],
            fullname=user['fullname'],
            password=self.hashPassword(user['password']),
            identity=identity,
            identity_pub=identity_pub,
            networks=[],
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
        print "save network:", network_id, "for user", user_id
        user['networks'].append(network_id)
        user = database.users.save(user)
        return self.success()

