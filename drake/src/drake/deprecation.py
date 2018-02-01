# Copyright (C) 2009-2018, Quentin "mefyl" Hocquet
#
# This software is provided "as is" without warranty of any kind,
# either expressed or implied, including but not limited to the
# implied warranties of fitness for a particular purpose.
#
# See the LICENSE file for more information.

import warnings
import functools

def deprecated(func):
  '''This is a decorator which can be used to mark functions
    as deprecated. It will result in a warning being emmitted
    when the function is used.'''
  @functools.wraps(func)
  def f(*args, **kwargs):
    warnings.warn(
      'Call to deprecated function {}.'.format(func.__name__),
      category=DeprecationWarning,
      stacklevel=2)
    return func(*args, **kwargs)
  return f
