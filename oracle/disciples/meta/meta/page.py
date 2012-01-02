# -*- encoding: utf-8 -*-

import web
import genshi

from meta.viewer import ViewerStore
from meta.session import users
from meta import conf

class Page(object):
    __viewers__ = ViewerStore()
    __skeleton__ = 'skeleton.html'
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
        if 'user' not in obj:
            obj['user'] = users.get_user()

        if 'path' not in obj:
            obj['path'] = web.ctx.path
        web.header('Content-Type', 'text/html')
        content = self.viewer.render(template, obj)
        if web.ctx.env.get('HTTP_X_REQUESTED_WITH') == 'XMLHttpRequest':
            return content
        else:
            var = {'html_body': genshi.HTML(content)}
            var.update(obj)
            return self.viewer.render(self.__skeleton__, var)

    def GET(self):
        return self.render()
