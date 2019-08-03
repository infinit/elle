# Copyright (C) 2019, Quentin "mefyl" Hocquet
#
# This software is provided "as is" without warranty of any kind,
# either expressed or implied, including but not limited to the
# implied warranties of fitness for a particular purpose.
#
# See the LICENSE file for more information.

'''OCaml menhir parser generator support.'''

import drake.ocaml

class MenhirSource(drake.ocaml.Node):

  '''Ocaml .mly menhir source file.'''

  def to_objects(self, toolkit, config):

    '''Convert to the associated implementation and interface file.'''

    builder = Menhir(self)
    return [builder.target_source, builder.target_interface]

drake.Node.extensions['mly'] = MenhirSource

class Menhir(drake.Builder):

  '''Menhir parser generator.'''

  def __init__(self, source):
    self.__source = source
    self.__target_source = drake.ocaml.Implementation(source.name().with_extension('ml'))
    self.__target_interface = drake.ocaml.Interface(source.name().with_extension('mli'))
    super().__init__([source], [self.__target_source, self.__target_interface])

  def execute(self):
    base = self.__target_source.path().without_last_extension()
    self.cmd('Menhir {}'.format(self.__target_source),
             ['menhir', self.__source, '--base', base],
             throw=True)
    return True

  @property
  def source(self):
    '''The menhir source file.'''
    return self.__source

  @property
  def target_source(self):
    '''The produced implementation source file.'''
    return self.__target_source

  @property
  def target_interface(self):
    '''The produced interface source file.'''
    return self.__target_interface
