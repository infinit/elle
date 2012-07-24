# -*- encoding: utf-8 -*-

import sys
import web

from meta import conf
from meta import resources
from meta import database
from meta.session import Session
from meta.session_store import SessionStore

class Application(object):
    """
    Application class wrap web.application.run method
    """
    def __init__(self, ip='127.0.0.1', port=12345):
        self.ip = ip
        self.port = port

        urls = []
        views = {}

        for resource in resources.ALL:
            id_ = str(id(resource))
            urls.extend([resource.__pattern__, id_])
            print("%s: %s" % (resource.__name__, resource.__pattern__))
            views[id_] = resource

        self.app = web.application(urls, views)
        session = Session(self.app, SessionStore(database.sessions()))
        for cls in views.itervalues():
            cls.__session__ = session

    def run(self):
        """
        Run the web server
        """
        sys.argv[1:] = [self.ip + ':' + str(self.port)]
        self.app.run()


