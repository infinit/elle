#
# ---------- imports ----------------------------------------------------------
#

import sys
import web

from meta import conf
from meta import pages
from meta import database
from meta.session import Session
from meta.session_store import SessionStore

#
# ---------- classes ----------------------------------------------------------
#

class Application(object):

    _urls = (
        '/', 'Root',
        '/account', 'Account',
        '/login', 'Login',
        '/logout', 'Logout',
        '/register', 'Register',
        '/user/(.+)', 'User',
        '/prototype/(.+)', 'Prototype',
    )

    _views = {
        'Root': pages.Root,
        'Login': pages.Login,
        'Logout': pages.Logout,
        'Register': pages.Register,
        'User': pages.User,
        'Prototype': pages.Prototype,
        'Account': pages.Account,
    }

    def __init__(self, ip='127.0.0.1', port=12345):
        self.ip = ip
        self.port = port
        self.app = web.application(self._urls, self._views)
        session = Session(self.app, SessionStore(database.sessions))
        for _, cls in self._views.iteritems():
            cls.__session__ = session

    def run(self):
        sys.argv[1:] = [self.ip + ':' + str(self.port)]
        self.app.run()
