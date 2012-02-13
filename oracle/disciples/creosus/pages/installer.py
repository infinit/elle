# -*- encoding: utf-8 -*-

import troll

import creosus

class Installer(creosus.Page):
    __template__ = 'installer.html'

    @troll.view.exposeWhen('install')
    def index(self):
        return self.render()
