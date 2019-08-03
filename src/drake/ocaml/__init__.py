# Copyright (C) 2019, Quentin "mefyl" Hocquet
#
# This software is provided "as is" without warranty of any kind,
# either expressed or implied, including but not limited to the
# implied warranties of fitness for a particular purpose.
#
# See the LICENSE file for more information.

'''OCaml compilation support.'''

import copy
import pipes
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
    self.__dependencies = {}

  def _compile_command(self, source, target, config):
    return self.__command('ocamlc', ['-c', source, '-o', target], config)

  def _link_command(self, sources, target, config):
    return self.__command('ocamlc', list(sources) + ['-o', target], config)

  def _dependencies_command(self, node, config):
    return self.__command('ocamldep', ['-one-line', node], config)

  def __command(self, binary, args, config):
    return [self.__find, binary] + list(self.__compile_flags(config)) + args

  def __compile_flags(self, config):
    return _flatten(['-I', dir] for dir in config.include_directories)

  def _register_dependency(self, depender, dependee):
    # ocamldep will report dependencies on cmi files. My hypothesis here is that this can be used to
    # deduce dependencies between associated cmo files.
    if isinstance(dependee, InterfaceObject):
      name = dependee.name().with_extension('cmo')
      # I'm not too sure it systematically exist yet, better safe than sorry.
      assert name in drake.Drake.current.nodes
      dependee = drake.node(name)
      if dependee is depender:
        # cmo depends on their cmi, don't create a loop
        return
    self.__dependencies.setdefault(depender, set()).add(dependee)

  def _dependencies_topological(self, nodes):
    dependencies = dict(
      (node, set(filter(lambda n: n in nodes, self.__dependencies.get(node, set()))))
      for node in nodes)
    while dependencies:
      free = None
      for k, values in dependencies.items():
        if not values:
          free = k
      if free is None:
        raise Exception('circular dependencies')
      for values in dependencies.values():
        values.discard(free)
      dependencies.pop(free)
      yield free

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

    kind = InterfaceObject if isinstance(self, Interface) else ImplementationObject
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
  elif not isinstance(node, Node):
    raise Exception('non-OCaml source for an OCaml binary: {}'.format(node))
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

  def __str__(self):
    return 'dependencies of {}'.format(self.__source)

  def execute(self):
    return self.cmd('Dependencies {}'.format(self.__target),
                    self.__toolkit._dependencies_command(self.__source, self.__config),
                    redirect_stdout=self.__target,
                    throw=True)

  def dependencies(self):

    # Unfortunately, as we rely on the `ocamldep` binary to find such dependencies, we need
    # generated ml/mli files to be available when it runs or they might be missed (eg a .mly needs
    # to be processed with menhir first so that a dependency on the parser will be seen by
    # ocamldep). Thus, we make all .deps files depend on all generated .ml files. This will probably
    # introduce superfluous dependencies, as not all files require all generated files. To
    # circumvent this, dependencies detection would have to be reimplemented in a
    # drake-generated-source aware fashion, as is done for cxx. However this requires a rather deep
    # semantic understanding of .ml files, much more than parsing '#include <...>'. The current
    # approach seems a minor setback and a more reasonable implementation for now.

    # The saddest part is that AFAICT the generated file need only exist so ocamldep picks up that
    # the module must be compiled, the content does not matter. Wild idea: simply touch those files
    # ?

    # The wild idea of just touching those files seems to be working. The whole comment is left here
    # for now as an explanation and to ease a potential rollback in case it's a not-so-great
    # idea. And also because I'm a bit proud and amused, admittedly.

    for node in drake.Drake.current.nodes.values():
      if isinstance(node, (Implementation, Interface)) and node.builder is not None:
        for include in self.__config.include_directories:
          if node.name().dirname() is include and not node.path().exists():
            self.output('touch {}'.format(pipes.quote(str(node))), pretty='Touch {}'.format(node))
            node.path().touch()

class Dependencies(drake.Node):

  '''Ocaml dependencies files as generated by ocamldep.'''


class Compiler(drake.Builder):

  '''OCaml bytecode compiler'''

  def __init__(self, source, target, toolkit, config):
    self.__config = config
    self.__source = source
    self.__deps = Dependencies(source.name().with_extension(source.name().extension + '.deps'))
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
        target = drake.Path(target)
        if self.__source.builder is None:
          target = target.without_prefix(drake.path_source())
        if target == self.__target.path():
          if sources:
            for source in sources.split(' '):
              dep = drake.node(source)
              self.__handle_ocamldep(dep)
              self.add_dynsrc('ocamldep', dep)
          return
    raise Exception('could not find {} in {}'.format(self.__target, self.__deps))

  def __handle_ocamldep(self, dep):
    self.__toolkit._register_dependency(self.__target, dep)

  @classmethod
  def _ocamldep_handler(cls, builder, path, type_, _):
    res = drake.node(path, type_)
    builder.__handle_ocamldep(res)
    return res

drake.Builder.register_deps_handler('ocamldep', Compiler._ocamldep_handler)

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
    sources = set(filter(lambda n: n.__class__ is not InterfaceObject, self.__sources))
    sources = self.__toolkit._dependencies_topological(sources)
    self.cmd('Compile {}'.format(self.__target),
             self.__toolkit._link_command(sources, self.__target, self.__config),
             throw=True)
    return True

class Executable(Node):

  '''OCaml bytecode executable.'''

  def __init__(self, path, sources, toolkit, config):
    super().__init__(path)
    Linker(sources, self, toolkit, config)
