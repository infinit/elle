# -*- encoding: utf-8 -*-

import json
import web

from meta.page import Page
from meta import conf, database
import meta.mail

import metalib
import pythia

class Search(Page):
    __pattern__ = "/user/search"

    def POST(self):
        text = self.data["text"]
        if len(text):
            return self.error("Searching text not implemented")

        users = database.users().find(fields=["_id"], limit=100)
        result = list(user['_id'] for user in users)

        return self.success({
            'users': result,
        })

class FromPublicKey(Page):
    __pattern__ = "/user/from_public_key"

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

INVITATION_SUBJECT = "Invitation to test Infinit!"
INVITATION_CONTENT = """
Dear Alpha tester,

Welcome to Infinit! You are one of the first individuals to get to try it out!

Remember that the version you will be using is still quite basic and only
functions in a local network at the moment. It may still have bugs and the
interface is still not in its public version. For now, file sharing is limited
to 30 MB and only the network owner can edit the files. Don’t worry though!
We’ll be updating all of this over the coming weeks.

You can report bugs or simply tell us what you think by sending us an email at
feedback@infinit.io or by visiting feedback.infinit.io. If you have any
questions, you can reach us on Skype at infinitdotio.

To get started, download Infinit here: http://infinit.io/download.

Your access code is: %(activation_code)s

If there are other people with whom you would like to use Infinit, please let
us know by sending an email to community@infinit.io.

Instructions:

 * MacOSX (only Mountain Lion):
    1) Install Infinit and create an account with your access code.
    2) Create a network.
    3) Drag and drop other users into your network.
    4) Drag and drop files into your network giving everyone read-only access
       to them.

 * Linux:
    1) Register a new account and start the Infinit shell:
        $ infinit register shell
    2) Start Infinit instances (in the Infinit shell):
        infinit-shell$ start
    3) Create a new network:
        infinit-shell$ create_network MyNetwork
    4) You might need to refresh networks with:
        infinit-shell$ refresh_networks
    5) Invite a friend to a network:
        infinit-shell$ invite my@friend.com MyNetwork

All the best,

--%(space)s
The Infinit Team
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
    __pattern__ = "/user/(.+)/view"

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
            'activation_code': dklds
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


