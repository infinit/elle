#!/usr/bin/env python2
# -*- encoding: utf-8 -*-

"""
longinus-server is an helper for hole punching
"""

import sys

from longinus.application import Application

if __name__ == '__main__':
    application = Application("0.0.0.0", 9999)
    application.run()
