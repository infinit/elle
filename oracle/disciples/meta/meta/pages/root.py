# -*- encoding: utf-8 -*-

from meta.page import Page

class Root(Page):
    __template__ = 'root.html'

    def GET(self):
        return self.render()
