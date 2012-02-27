# -*- encoding: utf-8 -*-

from pythia.client import Client

from pythia.constants import DEFAULT_SERVER, ADMIN_TOKEN

class Admin(Client):

    """
      An admin connection does nothing special except passing
      the ADMIN_TOKEN in requests
    """

    def __init__(self, server=DEFAULT_SERVER):
        Client.__init__(self, server, {})

    def post(self, url, params={}, token=None):
        params['admin_token'] = ADMIN_TOKEN
        print "ADMIN POST", params
        return Client.post(self, url, params, token)
