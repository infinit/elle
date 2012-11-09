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
        'email' : regexp.EmailValidator,
        'fullname': regexp.HandleValidator,
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

class SendFile(Page):
    """
    Send a file to a specific user.
    If you pass an email and the user is not registred in infinit,
    create a 'ghost' in the database, waiting for him to register.

    POST {
              'id_or_email': "email@pif.net", #required
              'file_name': "The file name", #required
              'network_id': "The network name", #required
              'file_size': 42 (ko) #optionnal
              'file_count': 32     #default 1
         }
         -> {
                'succes': True or False
            }
    """
    __pattern__ = "/user/sendfile"

    _validators = {
        'id_or_email': regexp.NonEmptyValidator,
        'file_name': regexp.FilenameValidator,
        'network_id': regexp.NetworkValidator,
    }

    def POST(self):
        self.requireLoggedIn()

        status = self.validate()
        if status:
            return self.error(*status)

        id_or_email = self.data['id_or_email'].strip()
        file_name = self.data['file_name'].strip()
        network_id = self.data['network_id'].strip()
        file_count = int(('file_count' in self.data and self.data['file_count'] or 1))
        file_size = int(('file_size' in self.data and self.data['file_size'] or -1))
        invitee_id = 0
        invitee_email = ""
        new_user = False
        invitee = 0

        # Determine if user sent a mail or an id.
        if re.match(regexp.Email, id_or_email):
            invitee_email = id_or_email
            # Check is user is in database.
            invitee = database.users().find_one({'email': id_or_email})
            # if the user doesn't exist, create a ghost and invite.
            if not invitee:
                new_user = True
                invitee_id = database.users().save({})
                self.registerUser(
                    _id = invitee_id,
                    email = invitee_email,
                    register_status = 'ghost',
                    notifications = [],
                    accounts=[{'type':'email', 'id':invitee_email}]
                )
            else:
                invitee_id = invitee['_id']
        elif not re.match(regexp.ID, id_or_email):
            return self.error(error.USER_ID_NOT_VALID)

        _id = self.user['_id']

        req = {'recipient': invitee_id,
               'sender': _id,
               'network_id': network_id}

        if not database.transactions().find_one(req):
            transaction_id = database.transactions().insert(req)

        sent = file_name + (file_count == 1 and "" or " and %i other files" % (file_count - 1))

        if not self.connected(invitee_id):
            if not invitee_email:
                invitee_email = database.users().find_one({'_id': id_or_email})['email']
            inviter_mail = self.user['email']

            subject = USER_INVITATION_SUBJECT % {
                'inviter_mail': inviter_mail,
            }

            content = (new_user and USER_INVITATION_CONTENT or USER_NEW_FILE_CONTENT) % {
                'inviter_mail': inviter_mail,
                'file_name': sent,
            }

            meta.mail.send(invitee_email, subject, content)

        self.notifier.notify_one(invitee_id,{
            'notification_id' : 7,
            'sender_id' : _id,
            'file_size': file_size,
            'file_count': file_count,
            'transaction_id' : transaction_id,
            'file_name': sent,
        })

        return self.success({'transaction_id': transaction_id})

class AnswerTransaction(Page):
    """
    Use to accept or deny a file transfer
    POST {
           'transaction_id' : the id on base of the transaction.
           'answer': True
         }
         -> {
                 'network_id': the network id or empty string if refused.
            }
    """
    __pattern__ = "/user/transaction"

    _validators = {
        'transaction_id': regexp.TransactionValidator,
    }

    def POST(self):
        self.requireLoggedIn()

        status = self.validate()
        if status:
            return self.error(*status)

        transaction =  database.transactions().find_one(
            database.ObjectId(self.data['transaction_id'].strip()))

        if not transaction:
            self.error(error.UNKNOWN, "This transaction doesn't exists.")

        self.notifier.notify_one(transaction['sender'],{
            '_id': 11,
            'network_id': transaction['network_id'],
            'status': int(self.data['status']),
        })

        database.transactions().remove(transaction)
        res = {'network_id': self.data['status'] and transaction['network_id']
                                                 or "",
        }

        return self.success({
                res,
        })

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
        'email': regexp.EmailValidator,
        'fullname': regexp.HandleValidator,
        'password': regexp.PasswordValidator,
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
            notifications = [],
            old_notifications = [],
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
                  'email': "test@infinit.io",
                  'password': "hashed_password",
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
        'email': regexp.EmailValidator,
        'password': regexp.PasswordValidator,
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

class Disconnection(Page):
    """
    POST {
              'user_id': "the user id".
              'user_token': 'the user token'
              'full': no more device for this client connected.
         }
         -> {
                 'success': True
            }
    """

    __pattern__ = "/user/disconnected"

    _validators = {
        'user_id': regexp.UserIDValidator,
    }

    def POST(self):
        if self.data['admin_token'] != pythia.constants.ADMIN_TOKEN:
            return self.error(error.UNKNOWN, "You're not admin")

        _id = self.data['user_id']
        token = self.data['user_token']

        del self.session.store[token]

        user = database.users().find_one(database.ObjectId(_id))

        database.users().update(
            {'_id': database.ObjectId(_id)},
            {'connected': bool(self.data['full'])},
        )

        return self.success()

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
