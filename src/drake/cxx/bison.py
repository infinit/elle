# Copyright (C) 2009-2011, Quentin "mefyl" Hocquet
#
# This software is provided "as is" without warranty of any kind,
# either expressed or implied, including but not limited to the
# implied warranties of fitness for a particular purpose.
#
# See the LICENSE file for more information.

from .. import Builder, Node, Path, node

class Bison:

  def __init__(self, bison = 'bison'):

    self.__bison = bison

  def plug(self, toolkit):

    toolkit.hook_bin_src_add(self.hook_bin_src)

  def hook_bin_src(self, src):

    if isinstance(src, BisonSource):
      builder = BisonCompiler(src, self)
      return builder.cc()

  def compile(self, path, dst):

    return '%s --defines --report=all -Dparse.error=verbose -Dlr.default-reductions=consistent --xml %s -o %s' % (self.__bison, path, dst)


class BisonSource(Node):

    def __init__(self, path):

      Node.__init__(self, path)

Node.extensions['y'] = BisonSource
Node.extensions['yy'] = BisonSource


class BisonCompiler(Builder):

  name = 'Bison compilation'

  def __init__(self, source, bison):

    self.__source = source
    self.__bison = bison

    base_path = source.src_path

    grammar_cc_path = Path(base_path)
    grammar_cc_path.extension = 'cc'
    self.__grammar_cc = node(grammar_cc_path)

    grammar_hh_path = Path(base_path)
    grammar_hh_path.extension = 'hh'
    self.__grammar_hh = node(grammar_hh_path)

    for base in ['location', 'position', 'stack']:
      path = base_path.dirname() / ('%s.hh' % base)
      self.__dict__['_BisonCompiler__%s' % base] = node(path)

    self.__grammar_hh = node(grammar_hh_path)

    Builder.__init__(self, [source],
                     [self.__grammar_cc,
                      self.__grammar_hh,
                      self.__location,
                      self.__position,
                      self.__stack,
                      ])

  def execute(self):

    return self.cmd('Bison %s' % self.__source,
                    self.__bison.compile(self.__source.path(),
                                         self.__grammar_cc.path()))

  def cc(self):

    return self.__grammar_cc
