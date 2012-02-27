# -*- encoding: utf-8 -*-

import troll

import pythia

class Page(troll.view.Page):
    __skeleton__ = 'skeleton.html'

    @property
    def api(self):
        return pythia.Client(
            server=self.app.conf['meta_server'],
            session=self.api_session
        )

    @property
    def api_admin(self):
        return pythia.Admin(
            server=self.app.conf['meta_server']
        )

    @property
    def api_session(self):
        return self.session.setdefault('api_session', {})

