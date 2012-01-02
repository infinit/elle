#
# ---------- imports ----------------------------------------------------------
#

import sys
import web

from meta import pages
from meta import session
from meta.database import db

#
# ---------- classes ----------------------------------------------------------
#

class Application(object):

    _urls = (
        '/', 'Root',
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
    }

    def __init__(self, ip='127.0.0.1', port=12345):
        self.ip = ip
        self.port = port
        self.app = web.application(self._urls, self._views)

        # init mongo store
        self.session = web.session.Session(self.app, session.MongoStore(db, 'sessions'))
        session.users.session = session
        session.users.collection = db.users
        session.users.SALTY_GOODNESS = u"1nf1n17_S4l7"

    def run(self):
        sys.argv[1:] = [self.ip + ':' + str(self.port)]
        self.app.run()
