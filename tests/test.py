#!/usr/bin/env python3

import doctest
import os
import os.path
import sys
import unittest

myself = os.getcwd()
dir = os.path.dirname(__file__)
if dir:
  myself += '/%s' % dir
sys.path = [myself + '/../src'] + sys.path
os.environ['PYTHONPATH'] = ':'.join(sys.path)

import drake
import drake.cxx
import drake.cxx.boost
import drake.git
import drake.go
import drake.python
import drake.utils
import sched

assert os.system('src/drake/threadpool-test.py') == 0
assert os.system('src/drake/sched-test.py') == 0

def test_suite():
  tests = []

  instance = [None]
  def setup(test):
    instance[0] = drake.Drake()
    instance[0].__enter__()

  def teardown(test):
    instance[0].__exit__(None, None, None)
    instance[0] = None

  tests = [doctest.DocTestSuite(module = m,
                                setUp = setup, tearDown = teardown)
           for m in [
               drake,
               drake.cxx,
               drake.cxx.boost,
               drake.git,
               drake.go,
               drake.python,
               drake.utils,
               sched,
           ]]
  return unittest.TestSuite(tests)

unittest.main(defaultTest='test_suite')
