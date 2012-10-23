# -*- encoding: utf-8 -*-

import web
import hashlib
import json
import traceback
import urllib

from meta import conf
from meta import database
from meta import notfification

class Page(object):
    """
    Base class for all page, simplifies the use of viewers.
    It also wrap access (and cache) to session and users in a lazy load manner
    """
    __session__ = None #set by the application

    __notifier = None

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
                self.__notifier = notification.TrophoniusNotify()
                self.open()
            except:
                pass
        return self._notifier

    @property
    def input(self):
        if self._input is None:
            self._input = web.input()
        return self._input

    def logout(self):
        self.session.kill()

    def authenticate(self, email, password):
        user = database.users().find_one({
            'email': email,
            'password': self.hashPassword(password)
        })
        if user:
            self._user = user
            self.session._user_id = user['_id']
            return True
        else:
            return False

    def registerUser(self, **kwargs):
        user = database.users().save(kwargs)
        return user

    def requireLoggedIn(self):
        if not self.user:
            raise web.Forbidden()

    def hashPassword(self, password):
        seasoned = password + conf.SALT
        seasoned = seasoned.encode('utf-8')
        return hashlib.md5(seasoned).hexdigest()

    def error(self, s):
        return json.dumps({
            'success': False,
            'error': str(s),
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
