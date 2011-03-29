# Copyright (C) 2011, Quentin "mefyl" Hocquet
#
# This software is provided "as is" without warranty of any kind,
# either expressed or implied, including but not limited to the
# implied warranties of fitness for a particular purpose.
#
# See the LICENSE file for more information.


import drake


class ConvertBuilder(drake.Builder):
  def __init__(self, source):
    self.__source = source
    path = drake.Path(source.name())
    path.extension = "eps"
    self.__target = drake.node(path)
    drake.Builder.__init__(self, [source] , [self.__target])

  def execute(self):
    return self.cmd(
      "Convert %s" % ( self.__target ),
      "convert %s %s", self.__source.path(), self.__target.path())
