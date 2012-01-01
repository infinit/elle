# -*- encoding: utf-8 -*-

from meta.viewer import ViewerStore
from meta import conf

import web

class Page(object):
    __viewers__ = ViewerStore()
    __template__ = None
    __template_dir__ = conf.TEMPLATE_DIR

    def __init__(self):
        self._viewer = None

    @property
    def viewer(self):
        if self._viewer is None:
            self._viewer = self.__viewers__.get(
                self.__template_dir__,
                conf.DEBUG,
                conf.ENCODING
            )
        return self._viewer

    def render(self, obj=None, template=None):
        if template is None:
            template = self.__template__
        assert template is not None
        if obj is None:
            obj = {}

        if 'path' not in obj:
            obj['path'] = web.ctx.path
        web.header('Content-Type', 'text/html')
        return self.viewer.render(template, obj)
