# Copyright (C) 2011, Quentin "mefyl" Hocquet
#
# This software is provided "as is" without warranty of any kind,
# either expressed or implied, including but not limited to the
# implied warranties of fitness for a particular purpose.
#
# See the LICENSE file for more information.


import drake


class ImageMagick(drake.Configuration):

  """Configuration for the ImageMagick program suite."""

  def __init__(self, magick_config = None, prefix = None):
    """Build a Convert object.

    magick_config -- Path to the Magick-config program.
    prefix        -- Prefix where the suite is installed.

    If neither magick_config nor prefix is provided,
    /usr/bin/Magick-config and /usr are used respectively.

    If magick_config is provided, it is used to determine the Image
    magick suite configuration.

    Otherwise, or if Magick-config failed to provide the configuration
    (i.e., the binary does not actually exist or doesn't work), prefix
    is used (i.e., binaries are searched in prefix/bin, etc).

    Otherwise, or if the prefix isn't valid (i.e., a beacon file,
    prefix/bin/convert, isn't found), configuration fails and an
    exception is thrown.
    """
    # Default configuration.
    if magick_config is None and prefix is None:
      magick_config = '/usr/bin/Magick-config'
      prefix = '/usr'
    # Prepare arguments
    if magick_config is not None:
      magick_config = drake.Path(magick_config)
    if prefix is not None:
      prefix = drake.Path(prefix)
    self.__bin = None
    # Try with Magick-config.
    if magick_config is not None:
      try:
        cmd = [str(magick_config), '--exec-prefix']
        self.__bin = drake.Path(drake.cmd_output(cmd).strip()) / 'bin'
      except:
        pass
    # Try with the prefix.
    if self.__bin is None and prefix is not None:
      self.__bin = prefix / 'bin'
    # Check validity
    if not self.convert().exists():
      raise Exception('%s does not exist' % self.convert())

  def convert(self):
    return self.__bin / 'convert'


class ConvertBuilder(drake.Builder):

  """Builder to convert between image formats using the 'convert' program."""

  def __init__(self, source, target_extension,
               imagemagick = ImageMagick()):
    """Create a ConvertBuilder.

    source           -- The source node.
    target_extension -- File extension to convert to.
    """
    path = drake.Path(source.name())
    path.extension = target_extension
    self.__imagemagick = imagemagick
    self.__source = source
    self.__target = drake.node(path)
    drake.Builder.__init__(self, [self.__source] , [self.__target])

  def execute(self):
    """Run convert."""
    return self.cmd("Convert %s" % self.__target,
                    "%s %s %s", self.__imagemagick.convert(),
                    self.__source.path(), self.__target.path())

  def target(self):
    """The node for the converted file."""
    return self.__target
