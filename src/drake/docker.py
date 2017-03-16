import collections
import drake
import json
import os
import shutil
import subprocess

from itertools import chain

def parents(p):
  while p != drake.Path.dot:
    yield p
    p = p.dirname()

def rootify(paths):
  res = set()
  for p in paths:
    insert = True
    for e in set(res):
      if e.prefix_of(p):
        insert = False
        break
      elif p.prefix_of(e):
        res.remove(e)
    if insert:
      res.add(p)
  return res

def installed_files(nodes):
  res = set()
  for node in nodes:
    res.add(node)
    for dep in node.dependencies_recursive:
      res.add(dep)
  return res

class DockerFile(drake.Node):

  def __init__(self, name, image, maintainer = None, labels = {}):
    super().__init__(name)
    self.__image = image
    self.__maintainer = maintainer
    self.__labels = labels
    self.__adds = {}
    self.__cmd = []
    self.__entry_point = []
    self.__ports = []
    self.__steps = []
    self.__volumes = []
    DockerFile.Builder(self)

  def add(self, nodes, path):
    if isinstance(nodes, collections.Iterable):
      for node in nodes:
        self.add(node, path)
    else:
      self.__adds.setdefault(drake.Path(path), []).append(nodes)

  def cmd(self, cmd):
    if isinstance(cmd, list):
      self.__cmd = cmd
    elif isinstance(cmd, str):
      self.__cmd = cmd.split()
    else:
      raise Exception('invalid CMD type: %s' % type(cmd))

  def entry_point(self, entry):
    if isinstance(entry, list):
      self.__entry_point = entry
    elif isinstance(entry, str):
      self.__entry_point = entry.split()
    else:
      raise Exception('invalid ENTRYPOINT type: %s' % type(cmd))

  def ports(self, ports):
    self.__ports = ports

  def volumes(self, volumes):
    if not isinstance(volumes, list):
      raise Exception('invalid VOLUMES type: %s', type(volumes))
    self.__volumes = volumes

  def run(self, cmd):
    self.__steps.append(('RUN', cmd))

  def env(self, k, v):
    self.__steps.append(('ENV', '{} {}'.format(k, v)))

  @property
  def image(self):
    return self.__image

  @property
  def maintainer(self):
    return self.__maintainer

  @property
  def labels(self):
    return self.__labels

  @property
  def adds(self):
    return chain(*self.__adds.values())

  @property
  def cmd_(self):
    return self.__cmd

  @property
  def entry_point_(self):
    return self.__entry_point

  @property
  def ports_(self):
    return self.__ports

  @property
  def steps(self):
    return self.__steps

  @property
  def volumes_(self):
    return self.__volumes

  def hash(self):
    return {
      'add': str(self.__adds),
      'cmd': self.__cmd,
      'entry-point': self.__entry_point,
      'image': self.__image,
      'labels': self.__labels,
      'maintainer': self.__maintainer,
      'ports': self.__ports,
      'steps': self.__steps,
      'volumes': self.__volumes,
    }

  class Builder(drake.Builder):

    def __init__(self, dockerfile):
      self.__dockerfile = dockerfile
      super().__init__([], [dockerfile])

    def execute(self):
      root = self.__dockerfile.path().dirname()
      self.output('Generate %s' % self.__dockerfile)
      with open(str(self.__dockerfile.path()), 'w') as f:
        print('FROM %s' % self.__dockerfile.image, file = f)
        if self.__dockerfile.maintainer is not None:
          print('MAINTAINER %s' % self.__dockerfile.maintainer,
                file = f)
        for k, v in self.__dockerfile.labels.items():
          print('LABEL %s="%s"' % (k, v), file = f)
        for cmd, args in self.__dockerfile.steps:
          print('{} {}'.format(cmd, args), file = f)
        for p, nodes in self.__dockerfile._DockerFile__adds.items():
          for add in rootify(
              chain(*(parents(n.path().without_prefix(root))
                      for n in installed_files(nodes)
                      if n is not drake.Path.dot))):
            print('ADD %s %s/%s'  % (add, p, add), file = f)
        for p in self.__dockerfile.ports_:
          print('EXPOSE %s' % p, file = f)
        if self.__dockerfile.volumes_:
          print('VOLUME %s' % json.dumps(self.__dockerfile.volumes_), file = f)
        if self.__dockerfile.entry_point_:
          print('ENTRYPOINT %s' % json.dumps(self.__dockerfile.entry_point_),
                file = f)
        if self.__dockerfile.cmd_:
          print('CMD %s' % json.dumps(self.__dockerfile.cmd_), file = f)
      return True

    def hash(self):
      return self.__dockerfile.hash()


class DockerImage(drake.BaseNode):

  def __init__(self, name, repository, tag):
    path = drake.Drake.current.prefix / name
    path = drake.Path(path._Path__path, False, True)
    super().__init__(path)
    self.__repository = repository
    self.__tag = tag

  def missing(self):
    i = subprocess.check_output(
      ['docker', 'images', '-q',
       '%s:%s' % (self.__repository, self.__tag)])
    return not bool(i)

  #def local_mtime(self):
  #  docker inspect  -f '{{ .Created }}' infinit:0.6.0-rc-169-g16efa2c

  @property
  def repository(self):
    return self.__repository

  @property
  def tag(self):
    return self.__tag


class DockerBuilder(drake.Builder):

  def __init__(self, image, docker_file):
    self.__image = image
    self.__docker_file = docker_file
    drake.Builder.__init__(
      self,
      chain(docker_file.adds, [self.__docker_file]),
      [self.__image])
    self.__path = self.__docker_file.path().dirname()

  def execute(self):
    # Cleanup
    effective = set()
    for path, dirs, files in os.walk(str(self.__path)):
      path = drake.Path(path)
      for f in chain(files, dirs):
        effective.add(path / f)
    expected = set(
      chain(*(parents(n.path())
              for n in installed_files(self.__docker_file.adds))))
    for g in rootify(p.without_prefix(self.__path)
                     for p in (effective - expected)
                     if p is not self.__docker_file.path()):
      g = self.__path / g
      self.output('Cleanup %s' % g)
      g = str(g)
      if os.path.isdir(g):
        shutil.rmtree(str(g))
      else:
        os.remove(g)
    tag = ':'.join((self.__image.repository, self.__image.tag))
    self.cmd('Docker build image %s' % tag,
             ['docker', 'build',
              '--force-rm',
              '--tag', tag,
               self.__path,
             ],
             throw = True)
    return True
