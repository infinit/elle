#!/usr/bin/env python2
# -*- encoding: utf-8 -*-

import os

META_LISTEN_TCP_PORT = int('META_SERVER_PORT' in os.environ and os.environ['META_SERVER_PORT'] or 12345)
TROPHONIUS_LISTEN_SSL_PORT = int('TROPHONIUS_NOTIFICATION_PORT' in os.environ and os.environ['TROPHONIUS_NOTIFICATION_PORT'] or 23457)
