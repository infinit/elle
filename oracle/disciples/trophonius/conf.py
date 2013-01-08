#!/usr/bin/env python2
# -*- encoding: utf-8 -*-

import os

LISTEN_TCP_PORT = int(os.environ.get('TROPHONIUS_SERVER_PORT', 23456))
LISTEN_SSL_PORT = int(os.environ.get('TROPHONIUS_NOTIFICATION_PORT', 23457))

SSL_KEY = "pkey"
SSL_CERT = "cert"
