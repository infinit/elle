# -*- encoding: utf-8 -*-

import json
import web

from meta.page import Page
from meta import conf, database
import meta.mail

import metalib
import pythia

class Search(Page):
    # XXX doc and improve

    __pattern__ = "/user/search"

    def POST(self):
        user = database.users().find_one({
            'public_key': self.data['public_key'],
        })

        if not user:
            return self.error("No user could be found for this public key!")
        return self.success({
            '_id': user['_id'],
            'email': user['email'],
            'public_key': user['public_key'],
            'fullname': user['fullname'],
        })


# TODO use mailchimp templates

INVITATION_SUBJECT = "Invitation to test Infinit !"
INVITATION_CONTENT = """
Dear early beta tester,

    This is an official invitation from Infinit, just download it for your
platform at http://download.infinit.io/ and paste your activation code into the
registration form.

Activation code: %(activation_code)s

--%(space)s
The infinit team
http://infinit.io

""".strip()

class Invite(Page):
    # XXX doc and improve

    __pattern__ = "/user/invite"

    def POST(self):
        if self.data['admin_token'] != pythia.constants.ADMIN_TOKEN:
            return self.error("You're not admin")
        email = self.data['email'].strip()
        force = self.data.get('force', False)
        send_email = not self.data.get('dont_send_email', False)
        if database.invitations().find_one({'email': email}):
            if not force:
                return self.error("Already invited!")
            else:
                database.invitations().remove({'email': email})
        code = self._generate_code(email)
        content = INVITATION_CONTENT % {
            'activation_code': code,
            'space': ' ',
        }
        if send_email:
            meta.mail.send(email, INVITATION_SUBJECT, content)
        database.invitations().insert({
            'email': email,
            'status': 'pending',
            'code': code,
        })
        return self.success()

    def _generate_code(self, mail):
        import hashlib
        import time
        hash_ = hashlib.md5()
        hash_.update(mail.encode('utf8') + str(time.time()))
        return hash_.hexdigest()

class Self(Page):
    """
    Get self infos
        GET
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
    """

    __pattern__ = "/self"

    def GET(self):
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

class One(Page):
    """
    Get public informations of an user by id or email
        GET
            -> {
                '_id': "id",
                'email': "email",
                'public_key': "public key in base64",
            }
    """
    __pattern__ = "/user/(.+)"

    def GET(self, id_or_email):
        if '@' in id_or_email:
            user = database.users().find_one({'email': id_or_email})
        else:
            user = database.byId(database.users(), id_or_email)
        if not user:
            return self.error("Couldn't find user for id '%s'" % str(id_or_email))
        return self.success({
            '_id': user['_id'],
            'email': user['email'],
            'public_key': user['public_key'],
            'fullname': user['fullname'],
        })

class Register(Page):
    """
    Register a new user
        POST {
            'email': "email@pif.net", #required
            'fullname': "The full name", #required
            'password': "password', #required
            'admin_token': 'admin token', #required
        }
    """

    __pattern__ = "/user/register"

    _validators = {
        'email': web.form.regexp(r".*@.*", "must be a valid email address"),
        'fullname': web.form.regexp(r".{3,90}$", 'fullname must be between 3 and 90 characters'),
        'password': web.form.regexp(r".{3,90}$", 'password must be between 3 and 90 characters'),
    }

    def POST(self):
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

        if not len(errors):
            if database.users().find_one({'email': user['email']}):
                errors.append('This email is already registered')
            elif user['activation_code'] != 'bitebite': # XXX
                invitation = database.invitations().find_one({
                    'code': user['activation_code'],
                    'status': 'pending',
                })
                if not invitation:
                    errors.append('Your activation code is wrong  ?!')
        if len(errors):
            return json.dumps({
                'success': False,
                'errors': errors,
                'error': ', '.join(errors),
            })

        user["_id"] = database.users().save({})

        identity, public_key = metalib.generate_identity(
            str(user["_id"]),
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
        if user['activation_code'] != 'bitebite': #XXX
            invitation['status'] = 'activated'
            database.invitations().save(invitation)
        return self.success()

class Login(Page):
    """
    Generate a token for further communication
        POST {
            "email": "mail !",
            "password": "password",
        }
            -> {
                'success': True,
                'token': "generated session token",
                'fullname': 'full name',
                'identity': 'Full base64 identity',
            }
    """
    __pattern__ = "/user/login"

    def POST(self):
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

class Logout(Page):
    """
    GET
        -> {
            'success': True
        }
    """

    __pattern__ = "/user/logout"

    def GET(self):
        if not self.user:
            return self.error("Not logged in")
        self.logout()
        return self.success()


