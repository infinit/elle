# Copyright (C) 2011, Quentin "mefyl" Hocquet
#
# This software is provided "as is" without warranty of any kind,
# either expressed or implied, including but not limited to the
# implied warranties of fitness for a particular purpose.
#
# See the LICENSE file for more information.

import os
import sys
import threading

DEBUG_TRACE = 1
DEBUG_TRACE_PLUS = 2
DEBUG_DEPS = 2
DEBUG_SCHED = 3

_DEBUG = 0
if 'DRAKE_DEBUG' in os.environ:
  _DEBUG = int(os.environ['DRAKE_DEBUG'])
_INDENT = 0
_DEBUG_SEM = threading.Semaphore(1)


def debug(msg, lvl = 1):
  if lvl <= _DEBUG:
    with _DEBUG_SEM:
      print >> sys.stderr, '%s%s' % (' ' * _INDENT * 2, msg)


class indentation:

  def __enter__(self):
    global _INDENT
    _INDENT += 1

  def __exit__(self, type, value, traceback):
    global _INDENT
    _INDENT -= 1
