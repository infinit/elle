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
        '/',                            'Root',
        '/me',                          'User',
        '/user/(.+)',                   'User',
        '/user_from_public_key',        'UserFromPublicKey',
        '/device/(.+)',                 'Device',
        '/devices',                     'Device',
        '/network/(.+)/(.+)',           'Network',
        '/network/(.+)',                'Network',
        '/network',                     'Network',
        '/networks',                    'Network',
        '/invite',                      'Invite',
    )

    _views = {
        'Root':                 pages.Root,
        'User':                 pages.User,
        'UserFromPublicKey':    pages.UserFromPublicKey,
        'Device':               pages.Device,
        'Network':              pages.Network,
        'Invite':               pages.Invite,
    }

    def __init__(self, ip='127.0.0.1', port=12345):
        self.ip = ip
        self.port = port
        self.app = web.application(self._urls, self._views)
        session = Session(self.app, SessionStore(database.sessions()))
        for cls in self._views.itervalues():
            cls.__session__ = session

    def run(self):
        """
        Run the web server
        """
        sys.argv[1:] = [self.ip + ':' + str(self.port)]
        self.app.run()


