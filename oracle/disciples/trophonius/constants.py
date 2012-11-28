#!/usr/bin/env python2
# -*- encoding: utf-8 -*-

import os

TROPHONIUS_LISTEN_TCP_PORT = int('TROPHONIUS_SERVER_PORT' in os.environ and os.environ['TROPHONIUS_SERVER_PORT'] or 23456)
TROPHONIUS_LISTEN_SSL_PORT = int('TROPHONIUS_NOTIFICATION_PORT' in os.environ and os.environ['TROPHONIUS_NOTIFICATION_PORT'] or 23457)

TROPHONIUS_SSL_KEY = "pkey"
TROPHONIUS_SSL_CERT = "cert"
