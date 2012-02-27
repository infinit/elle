# -*- encoding: utf-8 -*-

import troll
import creosus

class Root(creosus.Page):
    __template__ = 'root.html'

    @troll.view.expose
    def index(self):
        print self.session
        print self.session.permissions
        return self.render()
