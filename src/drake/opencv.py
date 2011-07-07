# Copyright (C) 2011, Quentin "mefyl" Hocquet
#
# This software is provided "as is" without warranty of any kind,
# either expressed or implied, including but not limited to the
# implied warranties of fitness for a particular purpose.
#
# See the LICENSE file for more information.

import drake
import drake.cxx

class OpenCV(drake.Configuration):

  """Configuration for the OpenCV library."""

  def __init__(self,
               prefix = None,
               version = drake.Version(),
               cxx_toolkit = None):
    """Build an OpenCV object.

    prefix -- Prefix where OpenCV is installed. It must contain, among
              other things, lib/libopencv_core.so.
    """
    # Fill default arguments.
    if prefix is not None:
      prefix = [drake.Path(prefix)]
    else:
      # pkg-config opencv --variable=prefix
      prefix = [drake.Path('/usr'), drake.Path('/usr/local')]
    cxx_toolkit = cxx_toolkit or drake.cxx.Toolkit()
    # Find the prefix
    beacon = drake.Path('include/opencv2/core/version.hpp')
    prefix = self._search(beacon, prefix)
    # Fill the configuration.
    self.__config = drake.cxx.Config()
    self.__config.add_system_include_path(prefix / 'include')
    self.__config.lib_path(prefix / 'lib')
    self.__config.lib('opencv_core')
    # Check the version
    output = cxx_toolkit.preprocess('# include<opencv2/core/version.hpp>\nCV_MAJOR_VERSION\nCV_MINOR_VERSION\nCV_SUBMINOR_VERSION')
    eff_version = drake.Version(*map(int, output.split('\n')[-4:-1]))
    if eff_version not in version:
      raise Exception('wrong OpenCV version, expected %s, got %s' % (version, eff_version))

  def config(self,
             calib3d = False,
             contrib = False,
             features2d = False,
             flann = False,
             gpu = False,
             highgui = False,
             imgproc = False,
             legacy = False,
             ml = False,
             objdetect = False,
             video = False
             ):
    res = drake.cxx.Config(self.__config)
    if objdetect:
      res.lib('opencv_objdetect')
    if highgui:
      res.lib('opencv_highgui')
    return res
