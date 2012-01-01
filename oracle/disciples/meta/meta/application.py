#
# ---------- imports ----------------------------------------------------------
#

import sys
import web

import pages

#
# ---------- classes ----------------------------------------------------------
#

class Application(object):

    URLs = (
        '/', 'Root',
        '/user/(.+)', 'User',
        '/prototype/(.+)', 'Prototype'
    )

    def __init__(self, ip='127.0.0.1', port=12345):
        self.ip = ip
        self.port = port
        self.app = web.application(self.URLs, {
            'Root': pages.Root,
            'User': pages.User,
            'Prototype': pages.Prototype,
        })

    def run(self):
        sys.argv[1:] = [self.ip + ':' + str(self.port)]
        self.app.run()
