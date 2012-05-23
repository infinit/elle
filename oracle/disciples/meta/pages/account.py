# -*- encoding: utf-8 -*-

import json

from meta.page import Page

class Account(Page):
    """
    Get account infos
        GET /account
            -> {
                'fullname': "My Name",
                'email': "My email",
                'devices': [device_id1, ...],
                'networks': [network_id1, ...]
            }

        TODO: Get other user information (identity public key)
    """

    def GET(self):
        if not self.user:
            return self.error("Not logged in")
        return self.success({
            'fullname': self.user['fullname'],
            'email': self.user['email'],
            'devices': self.user.get('devices', []),
            'networks': self.user.get('networks', []),
            'identity': self.user['identity'],
            'identity_pub': self.user['identity_pub'],
        })
