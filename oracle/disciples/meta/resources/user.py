# -*- encoding: utf-8 -*-

import json
import web

from meta.page import Page
from meta import conf, database
from meta import error
from meta import regexp

import meta.mail

import re

import metalib
import pythia

class Share(Page):
    __pattern__ = "/user/share"

    def POST(self):
        if self.notifier is not None:
            self.notifier.send_notify(**self.data)

class Search(Page):
    __pattern__ = "/user/search"

    def POST(self):
        text = self.data["text"]
        if len(text):
            return self.error(error.UNKNOWN, "Search is not implemented yet.")

        users = database.users().find(fields=["_id"], limit=100)
        result = list(user['_id'] for user in users)

        return self.success({
            'users': result,
        })

class GetSwaggers(Page):
    __pattern__ =  "/user/get_swaggers"

    def GET(self):
            return self.success({"swaggers" : self.user["swaggers"]})

class GetPendingSwaggers(Page):
    __pattern__ =  "/user/get_pending_swaggers"

    def GET(self):
        if "pending_swaggers" in self.user:
            return self.success({"pending_swaggers" : self.user["pending_swaggers"]})
        else:
            return self.error(error.UNKNOWN, "No swagger is pending.")

class AddSwagger(Page):
    __pattern__ = "/user/add_swagger"

    _validators = {
        'email': regexp.Validator(regexp.Email, error.EMAIL_NOT_VALID),
        'fullname': regexp.Validator(regexp.Handle, error.HANDLE_NOT_VALID),
    }

    def POST(self):
        import pymongo

        try:
            if "email" in self.data:
                error_code = _validators['email'](self.data['email'])
                if error_code:
                    return self.error(error_code)
                user = database.users().find_and_modify(
                    {"_id": pymongo.objectid.ObjectId(self.user["_id"])},
                    {"$addToSet": {"pending_swaggers": self.data["email"]}},
                )
                # XXX check for already exists.
                return self.success({"what" : "pending"})
            elif "_id" in self.data and \
                    self.user["_id"] != self.data["_id"]:
                swagger = database.byId(database.users(), self.data["_id"])
                if swagger:
                    user = database.users().find_and_modify(
                        {"_id" : pymongo.objectid.ObjectId(self.user["_id"])},
                        {"$addToSet" : {"swaggers" : self.data["_id"]}},
                    )
                else:
                    return self.error(error.UNKNOWN, "don't try to fake the swag")
        except Exception as e:
            return self.error(error.UNKNOWN, "your swag is too low !")
        return self.success({"swag":"up"})

class RemoveSwagger(Page):
    __pattern__ = "/user/remove_swagger"

    def POST(self):
        import pymongo

        swagez = database.users().find_and_modify(
            {"_id": pymongo.objectid.ObjectId(self.user["_id"])},
            {"$pull": {"swaggers": self.data["_id"]}},
            True #upsert
        )
        return self.success({"swaggers" : swagez["swaggers"]})

class FromPublicKey(Page):
    __pattern__ = "/user/from_public_key"

    def POST(self):
        user = database.users().find_one({
            'public_key': self.data['public_key'],
        })

        if not user:
            return self.error(error.UNKNOWN, "No user could be found for this public key!")
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
            return self.error(error.UNKNOWN, "You're not admin")
        email = self.data['email'].strip()
        force = self.data.get('force', False)
        send_email = not self.data.get('dont_send_email', False)
        if database.invitations().find_one({'email': email}):
            if not force:
                return self.error(error.UNKNOWN, "Already invited!")
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

USER_INVITATION_SUBJECT = "%(inviter_mail)s wants to share a file throught Infinit!"
USER_NEW_FILE_CONTENT = """
Dear user,

%(inviter_mail)s wants to share %(file_name)s.

--
The Infinit Team
http://infinit.io
""".strip()
USER_INVITATION_CONTENT = """
Dear user,

%(inviter_mail)s wants to share %(file_name)s and make you discover Infinit.

XXX
blabla, not stable, blabla, alpha, blabla, danger, blabla, destroy all your data, blabla.

To get started, download Infinit here: http://infinit.io/download.

IMPORTANT:
XXX
blabla, use this email, blabla

If there are other people with whom you would like to use Infinit, please let
us know by sending an email to community@infinit.io.

Instructions:

 * MacOSX (only Mountain Lion):
    1) Install Infinit and create an account with your access code.
    2) Roll a d100
    3) Add 50 cause you are a mac user
    4) Refer to table below to discover your anus breaking point.

| Result* >>>> Size
-----------------------------------------
| 51 - 55 >>>> IPod shuffle
| 56 - 60 >>>> IPod nano
| 61 - 65 >>>> IPod Classic
| 66 - 75 >>>> IPod Touch | IPhone 1,2,3
| 76 - 80 >>>> IPhone 4-5
| 81 - 85 >>>> IPad Mini
| 86 - 95 >>>> IPad
| 96 - 105 >>> Mac book mini
| 106 - 115 >> Mac book 13
| 116 - 125 >> Mac book 15
| 126 - 135 >> iMac 21
| 136 - 145 >> iMac 27
| 145 - 150 >> eMac

(*) Add 5 for each apple product you already purchased !

 * Linux:
    1) Wait for infinit to be integrated.

All the best,

--
The Infinit Team
http://infinit.io
""".strip()

class SendFile(Page):
    """
    Invite a new user by sending him a file. Generate a email and create a
    temporary user.
        POST {
            'id_or_email': "email@pif.net", #required
            'file_name': "The file name", #required
            'network_id': "The network name", #required
            'file_size': 42 (ko) #optionnal
            'file_count': 32     #default 1
             }
             ->
             {
             }
    """
    __pattern__ = "/user/sendfile"

    _validators = {
        'id_or_email': regexp.Validator(regexp.NotNull, error.FIELD_IS_EMPTY),
        'file_name': regexp.Validator(regexp.NotNull, error.FILE_NAME_EMPTY),
        'network_id': regexp.Validator(regexp.NetworkID, error.NETWORK_ID_NOT_VALID),
    }

    def POST(self):
        self.requireLoggedIn()

        status = self.validate()
        if status:
            return self.error(*status)

        id_or_email = self.data['id_or_email'].strip()
        file_name = self.data['file_name'].strip()
        network_id = self.data['network_id'].strip()
        file_count = ('file_count' in self.data and self.data['file_count'] or 1)
        invitee_id = 0
        invitee_email = ""
        new_user = False

        # Determine if user sent a mail or an id.
        if re.match(regexp.Email, id_or_email):
            invitee_email = id_or_email
            # Check is user is in database.
            invitee_id = database.users().find_one({'email': id_or_email})
            # if the user doesn't exist, create a ghost and invite.
            if not invitee_id:
                new_user = True
                invitee_id = database.users().save({})
                self.registerUser(
                    _id = invitee_id,
                    email = id_or_email,
                    register_status = 'ghost',
                    accounts=[{'type':'email', 'id':id_or_email}]
                )
            else:
                invitee_id = invitee_id['_id']
        elif not re.match(regexp.ID, id_or_email):
            return self.error(error.USER_ID_NOT_VALID)

        _id = self.user['_id']

        if not self.connected(invitee_id):
            inviter_mail = self.user['email']

            subject = USER_INVITATION_SUBJECT % {
                'inviter_mail': inviter_mail,
            }

            sent = file_name + (file_count == 1 and "" or " and %i other files" % (file_count - 1))

            content = (new_user and USER_INVITATION_CONTENT or USER_NEW_FILE_CONTENT) % {
                'inviter_mail': inviter_mail,
                'file_name': sent,
                }

            assert(invitee_email)
            meta.mail.send(invitee_email, subject, content)
        else:
            self.notifySwaggers(
                {
                  'notification_id' : 7,
                  '_id' : invitee_id,
                  'file_name': sent,
                }
            )

        req = {'recipient': invitee_id,
               'sender': _id,
               'network_id': network_id}

        if not database.transactions().find_one(req):
            database.transactions().insert(req)

        return self.success()

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
            return self.error(error.NOT_LOGGED_IN)
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
            return self.error(error.UNKNOWN_USER, "Couldn't find user for id '%s'" % str(id_or_email))
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
        'email': regexp.Validator(regexp.Email, error.EMAIL_NOT_VALID),
        'fullname': regexp.Validator(regexp.Handle, error.HANDLE_NOT_VALID),
        'password': regexp.Validator(regexp.Password, error.PASSWORD_NOT_VALID),
    }

    def POST(self):
        if self.user is not None:
            return self.error(error.ALREADY_LOGGED_IN)

        status = self.validate()
        if status:
            return self.error(*status)
        user = self.data

        if database.users().find_one({
                'accounts': [{ 'type': 'email', 'id':user['email']}],
                'register_status': 'ok',
           }):
            return self.error(error.EMAIL_ALREADY_REGISTRED)
        elif user['activation_code'] != 'bitebite': # XXX
            invitation = database.invitations().find_one({
                            'code': user['activation_code'],
                            'status': 'pending',
                            })
            if not invitation:
                return self.error(error.ACTIVATION_CODE_DOESNT_EXIST, "'%s' is not a valid activation code." % user['activation_code'])

        ghost = database.users().find_one({
                'accounts': [{ 'type': 'email', 'id':user['email']}],
                'register_status': 'ghost',
                })

        if ghost:
            user["_id"] = ghost['_id']
        else:
            user["_id"] = database.users().save({})

        identity, public_key = metalib.generate_identity(
            str(user["_id"]),
            user['email'], user['password'],
            conf.INFINIT_AUTHORITY_PATH,
            conf.INFINIT_AUTHORITY_PASSWORD
        )

        user_id = self.registerUser(
            _id=user["_id"],
            register_status='ok',
            email=user['email'],
            fullname=user['fullname'],
            password=self.hashPassword(user['password']),
            identity=identity,
            public_key=public_key,
            swaggers=[],
            pending_swaggers=[],
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

    _validators = {
        'email': regexp.Validator(regexp.Email, error.EMAIL_NOT_VALID),
        'password': regexp.Validator(regexp.Password, error.PASSWORD_NOT_VALID),
    }

    def POST(self):
        if self.user is not None:
            return self.error(error.ALREADY_LOGGED_IN)

        status = self.validate()
        if status:
            return self.error(*status)

        loggin_info = self.data

        if self.authenticate(loggin_info['email'], loggin_info['password']):
            self.notifySwaggers({"notification_id" : 8, "status" : 1})
            return self.success({
                "_id" : self.user["_id"],
                'token': self.session.session_id,
                'fullname': self.user['fullname'],
                'email': self.user['email'],
                'identity': self.user['identity'],
            })
        return self.error(error.EMAIL_PASSWORD_DONT_MATCH)

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
            return self.error(error.NOT_LOGGED_IN)
        self.logout()
        self.notifySwaggers({"notification_id": 8, "status" : 2})
        return self.success()
