# Copyright (C) 2019, Quentin "mefyl" Hocquet
#
# This software is provided "as is" without warranty of any kind,
# either expressed or implied, including but not limited to the
# implied warranties of fitness for a particular purpose.
#
# See the LICENSE file for more information.

'''OCaml compilation support.'''

import copy
import subprocess

import drake

def _flatten(nested):
  return (item for sublist in nested for item in sublist)

class Node(drake.Node):

  '''All OCaml-related nodes.'''

  def to_objects(self, toolkit, config):

    '''Convert to the associated object file.'''

    raise Exception('invalid source for an Ocaml binary: {}'.format(self))


class Toolkit:

  '''OCaml toolchain, defined by its `ocamlfind` binary.'''

  def __init__(self, ocamlfind='ocamlfind'):
    '''Create an OCaml toolchain from the given `ocamlfind` binary.'''
    self.__find = drake.Path(ocamlfind)

  def _compile_command(self, source, target, config):
    return self.__command('ocamlc', ['-c', source, '-o', target], config)

  def _link_command(self, sources, target, config):
    sources = filter(lambda n: n.__class__ is not InterfaceObject, sources)
    return self.__command('ocamlc', list(sources) + ['-o', target], config)

  def _dependencies_command(self, node, config):
    return self.__command('ocamldep', ['-one-line', node], config)

  def __command(self, binary, args, config):
    return [self.__find, binary] + list(self.__compile_flags(config)) + args

  def __compile_flags(self, config):
    return _flatten(['-I', dir] for dir in config.include_directories)

class Config:

  '''Configuration of an OCaml build.'''

  def __init__(self, source=None):
    '''An empty configuration or a copy of the given source.'''
    if source is not None:
      self.__include = copy.deepcopy(source.__include)
    else:
      self.__include = []

  def add_include_dir(self, directory):
    '''Add a directory to search modules in.'''
    self.__include.append(drake.path_build(directory))

  @property
  def include_directories(self):
    '''Directories to search modules in.'''
    return self.__include

class Source(Node):

  '''An OCaml source file.'''

  def to_objects(self, toolkit, config):

    '''Convert to the associated bytecode object file.'''

    kind = InterfaceObject if self.name().extension == 'mli' else ImplementationObject
    obj = kind(self.name().with_extension(self.extension))
    Compiler(self, obj, toolkit, config)
    return [obj]

  @property
  def extension(self):
    '''Extension once compiled.'''
    raise NotImplementedError()

class Implementation(Source):

  '''An OCaml .ml implementation file.'''

  @property
  def extension(self):
    return 'cmo'

drake.Node.extensions['ml'] = Implementation


class Interface(Source):

  '''An OCaml .mli interface file.'''

  @property
  def extension(self):
    return 'cmi'

drake.Node.extensions['mli'] = Interface


class Object(Node):

  '''An OCaml bytecode object file.'''


class ImplementationObject(Object):

  '''An OCaml .cmo bytecode implementation object file.'''

drake.Node.extensions['cmo'] = ImplementationObject


class InterfaceObject(Object):

  '''An OCaml .cmi bytecode interface object file.'''


drake.Node.extensions['cmi'] = InterfaceObject

def _to_objects(node, toolkit, config):
  if isinstance(node, Object):
    return [node]
  else:
    return _flatten(_to_objects(o, toolkit, config) for o in node.to_objects(toolkit, config))

class Ocamldep(drake.Builder):

  '''Builder for ocaml depedency files.'''

  def __init__(self, source, target, toolkit, config):
    self.__config = config
    self.__source = source
    self.__target = target
    self.__toolkit = toolkit
    super().__init__([source], [target])

  def execute(self):
    return self.cmd('Dependencies {}'.format(self.__target),
                    self.__toolkit._dependencies_command(self.__source, self.__config),
                    redirect_stdout=self.__target,
                    throw=True)

class Compiler(drake.Builder):

  '''OCaml bytecode compiler'''

  def __init__(self, source, target, toolkit, config):
    self.__config = config
    self.__source = source
    self.__deps = drake.node(source.name().with_extension(source.name().extension + '.deps'))
    Ocamldep(self.__source, self.__deps, toolkit, config)
    self.__target = target
    self.__toolkit = toolkit
    super().__init__([self.__deps, self.__source], [self.__target])

  def execute(self):
    self.cmd('Compile {}'.format(self.__target),
             self.__toolkit._compile_command(self.__source, self.__target, self.__config),
             throw=True)
    return True

  def dependencies(self):
    with open(str(self.__deps.path()), 'r') as deps:
      for line in deps:
        target, sources = map(str.strip, line.split(':'))
        if target == str(self.__target.path()):
          for source in sources.split(' '):
            self.add_dynsrc('ocamldep', drake.node(source))
          return
    raise Exception('could not find {} in {}'.format(self.__target, self.__deps))

class Linker(drake.Builder):

  '''OCaml bytecode linker.'''

  def __init__(self, sources, target, toolkit, config):
    sources = list(_flatten(_to_objects(source, toolkit, config) for source in sources))
    self.__config = config
    self.__sources = sources
    self.__target = target
    self.__toolkit = toolkit
    super().__init__(sources, [target])

  def execute(self):
    self.cmd('Compile {}'.format(self.__target),
             self.__toolkit._link_command(self.__sources, self.__target, self.__config),
             throw=True)
    return True

class Executable(Node):

  '''OCaml bytecode executable.'''

  def __init__(self, path, sources, toolkit, config):
    super().__init__(path)
    Linker(sources, self, toolkit, config)
