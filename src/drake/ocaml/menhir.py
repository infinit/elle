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
    self.__strict = True
    self.__source = source
    self.__target_source = drake.ocaml.Implementation(source.name().with_extension('ml'))
    self.__target_interface = drake.ocaml.Interface(source.name().with_extension('mli'))
    super().__init__([source], [self.__target_source, self.__target_interface])

  def execute(self):
    base = self.__target_source.path().without_last_extension()
    cmd = ['menhir', self.__source, '--base', base]
    if self.__strict:
      cmd.append('--strict')
    self.cmd('Menhir {}'.format(self.__target_source),
             cmd,
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

  @property
  def strict(self):
    '''Whether to pass `--strict` to menhir, making all grammar warnings errors.'''
    return self.__strict

  @strict.setter
  def strict(self, value: bool):
    self.__strict = value
