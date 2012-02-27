# -*- encoding: utf-8 -*-

import web

from meta import conf

class Session(web.session.Session):
    """
    This is a dirty hack to have a chance to get session id from token
    """

    def _load(self):
        """Load the session from the store, by the id from cookie"""
        cookie_name = self._config.cookie_name
        cookie_domain = self._config.cookie_domain
        self.session_id = web.cookies().get(cookie_name)

        ################################ ADDED PART HERE
        if self.session_id is None:
            self.session_id = web.ctx.env.get('HTTP_' + conf.SESSION_HEADER_NAME.upper())
        if self.session_id is None:
            self.session_id = web.input().get(conf.SESSION_TOKEN_NAME)
        #print "FOUND SESSION ID", self.session_id
        ################################


        # protection against session_id tampering
        if self.session_id and not self._valid_session_id(self.session_id):
            self.session_id = None

        self._check_expiry()
        if self.session_id:
            d = self.store[self.session_id]
            self.update(d)
            self._validate_ip()

        if not self.session_id:
            self.session_id = self._generate_session_id()

            if self._initializer:
                if isinstance(self._initializer, dict):
                    self.update(self._initializer)
                elif hasattr(self._initializer, '__call__'):
                    self._initializer()

        self.ip = web.ctx.ip
