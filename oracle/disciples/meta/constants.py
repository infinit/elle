#!/usr/bin/env python2
# -*- encoding: utf-8 -*-

import os

META_LISTEN_TCP_PORT = int(os.environ.get('META_SERVER_PORT', 12345))
TROPHONIUS_LISTEN_SSL_PORT = int(os.environ.get('TROPHONIUS_NOTIFICATION_PORT', 23457))
