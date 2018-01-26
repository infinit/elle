# Copyright (C) 2009-2018, Quentin "mefyl" Hocquet
#
# This software is provided "as is" without warranty of any kind,
# either expressed or implied, including but not limited to the
# implied warranties of fitness for a particular purpose.
#
# See the LICENSE file for more information.

import drake
import subprocess

def _find_command(t, v):
  if v is False:
    return None
  elif v is True or v is None:
    try:
      return t()
    except Exception as e:
      if v is True:
        raise
      else:
        return None
  else:
    return t(v)

class Command:

  __drake_configure__ = _find_command

  def __init__(self, path = None):
    if isinstance(path, self.__class__):
      self.__path = drake.Path(path.__path)
      self.__version = drake.Version(path.__version)
    else:
      if path is None:
        self.__path = drake.Path(self.__class__.name)
      else:
        self.__path = drake.Path(path)
      try:
        output = self._get_version()
      except Exception as e:
        raise Exception('Unable to find %s' % self.path) from e
      try:
        self.__version = self._parse_version(output)
      except Exception as e:
        raise Exception('Unable to parse %s version from %r' % \
                        (self.__class.name, output)) from e

  def _get_version(self):
    return subprocess.check_output(
      [str(self.path), '--version']).decode()

  def _parse_version(self, v):
    return drake.Version(v)

  @property
  def path(self):
    return self.__path

  @property
  def version(self):
    return self.__version
