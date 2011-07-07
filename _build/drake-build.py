#!/usr/bin/env python3

import os
import sys

sys.path.append('../src')
if 'PYTHONPATH' in os.environ:
  os.environ['PYTHONPATH'] = '../src:%s' % os.environ['PYTHONPATH']
else:
  os.environ['PYTHONPATH'] = '../src'

import drake

drake.run('..')
