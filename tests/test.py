#!/usr/bin/env python3

import os
import os.path
import sys

myself = os.getcwd()
dir = os.path.dirname(__file__)
if dir:
  myself += '/%s' % dir
sys.path = [myself + '/../src'] + sys.path
os.environ['PYTHONPATH'] = ':'.join(sys.path)

import doctest
import drake
import drake.git
import drake.python
import drake.utils

doctest.testmod(drake)
doctest.testmod(drake.git)
doctest.testmod(drake.python)
doctest.testmod(drake.utils)

assert os.system('src/drake/sched.py') == 0
os.chdir('_build')
assert os.system('./drake-build.py //check') == 0
