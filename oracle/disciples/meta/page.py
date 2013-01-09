
# -*- encoding: utf-8 -*-

import web
import hashlib
import json
import traceback
import urllib

import error
from meta import conf
from meta import database
from meta import notifier
from meta import error
from meta import regexp

class Page(object):
    """
    Base class for all page, simplifies the use of viewers.
    It also wrap access (and cache) to session and users in a lazy load manner
    """
    __session__ = None #set by the application

    __notifier = None

    _validators = []

    _mendatory_fields = []

    def __init__(self):
        self._input = None
        self._user = None

    @property
    def session(self):
        assert self.__session__ is not None
        return self.__session__

    @property
    def user(self):
        if self._user is None:
            try:
                self._user = database.users().find_one({
                    '_id': database.ObjectId(self.session._user_id)
                })
            except AttributeError:
                return None
        return self._user

    @property
    def notifier(self):
        if self.__notifier is None:
            try:
                self.__notifier = notifier.TrophoniusNotify()
                self.__notifier.open()
            except Exception as e:
                print(e)
                return self.__notifier
        return self.__notifier

    @property
    def input(self):
        if self._input is None:
            self._input = web.input()
        return self._input

    def validate(self):
        for (field, validator) in self._validators:
            if not field in self.data.keys():
                return (error.BAD_REQUEST, "Field %s is mandatory" % field)
            else:
                error_code = validator(self.data[field])
                if error_code:
                    return (error_code, str())
        for (field, type_) in self._mendatory_fields:
            if not field in self.data.keys() or not isinstance(self.data[field], type_):
                return (error.BAD_REQUEST, "Field %s is mandatory and must be an %s" % (field, type_))
        return ()

    def logout(self):
        print("user %s logged out.", self.user)
        self.session.kill()

    def authenticate(self, email, password):
        user = database.users().find_one({
            'email': email,
            'password': self.hashPassword(password)
        })
        if user:
            user['connected'] = True
            database.users().save(user)
            self._user = user
            self.session._user_id = user['_id']
            self.notifySwaggers(
                notifier.USER_STATUS,
                {
                    'status': 1,
                }
            )
            return True
        else:
            return False

    def registerUser(self, **kwargs):
        kwargs['connected'] = False
        user = database.users().save(kwargs)
        return user

    @staticmethod
    def connected(user_id):
        assert isinstance(user_id, database.ObjectId)
        user = database.users().find_one(user_id)
        if not user:
            raise Exception("This user doesn't exist")
        return user['connected']

    def forbidden(self, msg):
        raise web.HTTPError("403 {}".format(msg))
    def requireLoggedIn(self):
        if not self.user:
            self.forbidden("Authentication required.")

    def hashPassword(self, password):
        seasoned = password + conf.SALT
        seasoned = seasoned.encode('utf-8')
        return hashlib.md5(seasoned).hexdigest()

    def notifySwaggers(self, notification_id, data, bAll = False):
        swgs = list(self.user["swaggers"].keys())
        # if not bAll, notify only the connected ones.
        if not bAll:
            for s in swgs:
                if not self.connected(database.ObjectId(s)):
                    swgs.remove(s)
        d = {
                "user_id" : self.user["_id"],
            }
        d.update(data)
        self.notifier.notify_some(notification_id,
                                  swgs,
                                  d)

    def error(self, err=error.UNKNOWN, msg=""):
        assert isinstance(err, int)
        assert isinstance(msg, str)
        if not msg and err in error.error_details:
            msg = error.error_details[err]

        return json.dumps({
            'success': False,
            'error_code': err,
            'error_details': str(msg),
        })

    def success(self, obj={}):
        assert(isinstance(obj, dict))
        d = {'success': True}
        d.update(obj)
        return json.dumps(d, default=str)

    _data = None
    @property
    def data(self):
        if self._data is None:
            try:
                data = web.data()
                data = urllib.unquote(data)
                self._data = json.loads(data)
            except:
                traceback.print_exc()
                print "Cannot decode", data, web.data()
                raise ValueError("Wrong post data")
        return self._data
