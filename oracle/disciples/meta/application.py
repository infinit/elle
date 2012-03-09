# -*- encoding: utf-8 -*-

import sys
import web

from meta import conf
from meta import pages
from meta import database
from meta.session import Session
from meta.session_store import SessionStore

class Application(object):
    """
    Application class wrap web.application.run method
    """

    _urls = (
        '/', 'Root',
        '/account', 'Account',
        #'/user/(.+)', 'User',
        '/login', 'Login',
        '/logout', 'Logout',
        '/register', 'Register',
        '/device/(.+)', 'Device',
        '/devices', 'Device',
        '/network/(.+)/(.+)', 'Network',
        '/network/(.+)', 'Network',
        '/networks', 'Network',
    )

    _views = {
        'Root': pages.Root,
        'Login': pages.Login,
        'Logout': pages.Logout,
        'Register': pages.Register,
        'Account': pages.Account,
        'Device': pages.Device,
        'Network': pages.Network,
    }

    def __init__(self, ip='127.0.0.1', port=12345):
        self.ip = ip
        self.port = port
        self.app = web.application(self._urls, self._views)
        session = Session(self.app, SessionStore(database.sessions))
        for cls in self._views.itervalues():
            cls.__session__ = session

    def run(self):
        """
        Run the web server
        """
        sys.argv[1:] = [self.ip + ':' + str(self.port)]
        self.app.run()


