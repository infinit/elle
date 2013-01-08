#!/usr/bin/env python2
# -*- encoding: utf-8 -*-

import os

TROPHONIUS_LISTEN_TCP_PORT = int(os.environ.get('TROPHONIUS_SERVER_PORT', 23456))
TROPHONIUS_LISTEN_SSL_PORT = int(os.environ.get('TROPHONIUS_NOTIFICATION_PORT', 23457))

TROPHONIUS_SSL_KEY = "pkey"
TROPHONIUS_SSL_CERT = "cert"
