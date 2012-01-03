# -*- encoding: utf-8 -*-

import web
import genshi
import hashlib

from meta.viewer import ViewerStore
from meta import conf
from meta import database

class Page(object):
    """
    Base class for all page, simplifies the use of viewers.
    It also wrap access (and cache) to session and users in a lazy load manner
    """
    __viewers__ = ViewerStore()
    __skeleton__ = 'skeleton.html'
    __template__ = None
    __template_dir__ = conf.TEMPLATE_DIR

    __session__ = None #set by the application

    def __init__(self):
        self._viewer = None
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
                self._user = database.users.find_one({
                    '_id': self.session._user_id
                })
            except AttributeError:
                return None
        return self._user

    @property
    def input(self):
        if self._input is None:
            self._input = web.input()
        return self._input

    @property
    def viewer(self):
        if self._viewer is None:
            self._viewer = self.__viewers__.get(
                self.__template_dir__,
                conf.DEBUG,
                conf.ENCODING
            )
        return self._viewer

    def logout(self):
        self.session.kill()

    def authenticate(self, email, password):
        user = database.users.find_one({
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
        user = database.users.save(kwargs)
        return user


    def render(self, obj=None, template=None):
        if template is None:
            template = self.__template__
        assert template is not None
        if obj is None:
            obj = {}
        if 'user' not in obj:
            obj['user'] = self.user

        if 'path' not in obj:
            obj['path'] = web.ctx.path
        web.header('Content-Type', 'text/html')
        content = self.viewer.render(template, obj)
        if web.ctx.env.get('HTTP_X_REQUESTED_WITH') == 'XMLHttpRequest':
            return content
        else:
            var = {'html_body': genshi.HTML(content)}
            var.update(obj)
            return self.viewer.render(self.__skeleton__, var)

    def GET(self):
        return self.render()

    def hashPassword(self, password):
        seasoned = password + conf.SALT
        seasoned = seasoned.encode('utf-8')
        return hashlib.md5(seasoned).hexdigest()
