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
import drake.cxx
import drake.cxx.boost
import drake.git
import drake.python
import drake.utils
import sched

assert os.system('src/drake/sched-test.py') == 0

os.chdir('_build')
assert os.system('./drake-build.py //check') == 0

assert(doctest.testmod(sched)[0] == 0)
assert(doctest.testmod(drake)[0] == 0)
assert(doctest.testmod(drake.cxx.boost)[0] == 0)
assert(doctest.testmod(drake.git)[0] == 0)
assert(doctest.testmod(drake.python)[0] == 0)
assert(doctest.testmod(drake.utils)[0] == 0)
