# -*- encoding: utf-8 -*-

from pythia.client import Client

from pythia.constants import DEFAULT_SERVER, ADMIN_TOKEN

class Admin(Client):

    def __init__(self, server=DEFAULT_SERVER):
        Client.__init__(self, server, {
            'token': ADMIN_TOKEN,
        })

