# Copyright (C) 2009-2011, Quentin "mefyl" Hocquet
#
# This software is provided "as is" without warranty of any kind,
# either expressed or implied, including but not limited to the
# implied warranties of fitness for a particular purpose.
#
# See the LICENSE file for more information.

from .. import Builder, Node, Path, node


class Flex:

  def __init__(self, flex = 'flex'):

    self.__flex = flex

  def plug(self, toolkit):

    toolkit.hook_bin_src_add(self.hook_bin_src)

  def hook_bin_src(self, src):

    if isinstance(src, FlexSource):
      builder = FlexCompiler(src, self)
      return builder.cc()

  def compile(self, src, dst):

    return '%s -+  -o%s -Ca %s' % (self.__flex, dst, src)


class FlexSource(Node):

    def __init__(self, path):

      Node.__init__(self, path)

Node.extensions['l'] = FlexSource
Node.extensions['ll'] = FlexSource


class FlexCompiler(Builder):

  name = 'Flex compilation'

  def __init__(self, source, flex):

    self.__source = source
    self.__flex = flex

    base_path = source.name()
    scanner_cc_path = Path(base_path)
    scanner_cc_path.extension = 'cc'
    self.__scanner_cc = node(scanner_cc_path)
    Builder.__init__(self, [source], [self.__scanner_cc])

  def execute(self):

    if not self.cmd('Flex %s' % self.__source,
                    self.__flex.compile(self.__source.path(),
                                        self.__scanner_cc.path())):
      return False
    return self.cmd('Fixing flex include',
                    'sed -i s,FlexLexer.h,parser/flex-lexer.hh, %s' % self.__scanner_cc)

  def cc(self):

    return self.__scanner_cc
