# -*- encoding: utf-8 -*-

import web
import troll
import creosus

class Logout(creosus.Page):

    @troll.view.exposeWhen('logout')
    def index(self):
        self.app.session_hash = ''
        raise web.seeother('/')
