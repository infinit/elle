# -*- encoding: utf-8 -*-

import os
import platform

CURRENT_FILE_DIR = os.path.abspath(os.path.dirname(__file__))

BUILD = os.environ.get('DIR_BUILD')
if not BUILD:
    BUILD = os.path.join(CURRENT_FILE_DIR, '../../build/')
    platform_dir = platform.system().lower() + platform.architecture()[0][:2]
    if os.path.isdir(os.path.join(BUILD, platform_dir)):
        BUILD = os.path.join(BUILD, platform_dir)

SOURCE = os.environ.get('DIR_SOURCE')
if not SOURCE:
    SOURCE = os.path.join(CURRENT_FILE_DIR, '../..')
