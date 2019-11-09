# Copyright (C) 2019, Quentin "mefyl" Hocquet
#
# This software is provided "as is" without warranty of any kind,
# either expressed or implied, including but not limited to the
# implied warranties of fitness for a particular purpose.
#
# See the LICENSE file for more information.

'''OCaml ocamllex scanner generator support.'''

import drake.ocaml

class OcamllexSource(drake.ocaml.Node):

  '''Ocaml .mly ocamllex source file.'''

  def to_objects(self, toolkit, config):

    '''Convert to the associated implementation and interface file.'''

    builder = Ocamllex(self)
    return [builder.target]

drake.Node.extensions['mll'] = OcamllexSource

class Ocamllex(drake.Builder):

  '''Ocamllex scanner generator.'''

  def __init__(self, source):
    self.__source = source
    self.__target = drake.ocaml.Implementation(source.name().with_extension('ml'))
    super().__init__([source], [self.__target])

  def execute(self):
    self.cmd('Ocamllex {}'.format(self.__target),
             ['ocamllex', self.__source, '-o', self.__target.path().with_extension('ml')],
             throw=True)
    return True

  @property
  def source(self):
    '''The ocamllex source file.'''
    return self.__source

  @property
  def target(self):
    '''The produced implementation source file.'''
    return self.__target
