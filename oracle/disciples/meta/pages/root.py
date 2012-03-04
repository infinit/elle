# -*- encoding: utf-8 -*-

import json

from meta.page import Page

class Root(Page):
    """
    Give basic server infos
        GET /
            -> {
                "server": "server name",
                "logged_in": True/False,
            }
    """

    def GET(self):
        return self.success({
            'server': 'Meta 0.1',
            'logged_in': self.user is not None,
        })
