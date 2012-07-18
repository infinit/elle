# -*- encoding: utf-8 -*-

import json
import web

from meta.page import Page
from meta import conf, database
import metalib
import pythia

class UserFromPublicKey(Page):

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
            self._send_invitation(email, INVITATION_SUBJECT, content)
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

    def _send_invitation(self, mail, subject, content):
        from email.mime.text import MIMEText
        from email.header import Header
        #from email.utils import parseaddr, formataddr
        import smtplib
        msg = MIMEText(content, _charset='utf8')
        msg['Subject'] = Header(subject, 'utf8')
        msg['From'] = Header("Infinit.io <no-reply@infinit.io>", 'utf8')
        msg['To'] = Header(mail, 'utf8')

        smtp_server = smtplib.SMTP(conf.MANDRILL_SMTP_HOST, conf.MANDRILL_SMTP_PORT)
        smtp_server.login(conf.MANDRILL_USERNAME, conf.MANDRILL_PASSWORD)
        smtp_server.sendmail(msg['From'], [msg['To']], msg.as_string())
        smtp_server.quit()


class User(Page):
    """
    Get self infos
        GET /self
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

    Get public informations of an user by id or email
        GET /user/id_or_email
            -> {
                '_id': "id",
                'email': "email",
                'public_key': "public key in base64",
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
        return self._user_public(action)

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

    def _user_public(self, id_or_email):
        if '@' in id_or_email:
            user = database.users().find_one({'email': id_or_email})
        else:
            user = database.byId(database.users(), id_or_email)
        if not user:
            return self.error("Couldn't find user for id '%s'" % str(_id))
        return self.success({
            '_id': user['_id'],
            'email': user['email'],
            'public_key': user['public_key'],
            'fullname': user['fullname'],
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
        'password': web.form.regexp(r".{3,90}$", 'password must be between 3 and 90 characters'), #XXX min password size
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

        user["_id"] = str(database.users().save({}))

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
        if user['activation_code'] != 'bitebite': #XXX
            invitation['status'] = 'activated'
            database.invitations().save(invitation)
        return self.success()
