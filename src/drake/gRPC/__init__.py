import drake

class Generator(drake.Builder):
  '''Base builder for generating protobuf serialization and gRPC files from a
  .proto file.
  '''
  def __init__(self, proto, protoc, targets, plugin = None):
    self.__proto = proto
    if protoc is None:
      self.__protoc = drake.node(drake.which.which('protoc'))
    self.__protoc = protoc
    self.__plugin = plugin
    self.__output = proto.name().dirname()
    srcs = [proto, protoc]
    if plugin:
      srcs.append(plugin)
    super().__init__(srcs, targets)

  @property
  def proto(self):
    return self.__proto

  @property
  def protoc(self):
    return self.__protoc

  @property
  def plugin(self):
    return self.__plugin

  @property
  def output_dir(self):
    return drake.path_build(self.__output)

class CxxGen(Generator):
  '''Generate the C++ protobuf serialization and gRPC files from a .proto.

  Resulting files for <xxx.proto> will be:
  - xxx.pb.h
  - xxx.pb.cc
  - xxx.grpc.pb.h
  - xxx.grpc.pb.cc
  '''

  def __init__(self, proto, protoc = None, plugin = None):
    name = proto.name_relative
    dsts = drake.nodes(
      name.with_extension('pb.h'),
      name.with_extension('pb.cc'),
      name.with_extension('grpc.pb.h'),
      name.with_extension('grpc.pb.cc'),
    )
    if plugin is None:
      plugin = drake.node('/usr/local/bin/grpc_cpp_plugin')
    super().__init__(proto, protoc = protoc, plugin = plugin, targets = dsts)

  def execute(self):
    self.cmd('GRPC protoc %s' % self.proto, self.protoc_grpc_cmd, throw = True)
    self.cmd('GRPC protoc cc %s' % self.proto, self.protoc_cc_cmd, throw = True)
    return True

  @property
  def protoc_grpc_cmd(self):
    return [
      self.protoc,
      '-I', self.proto.path().dirname(),
      '--grpc_out=%s' % self.output_dir,
      '--plugin=protoc-gen-grpc=%s' % self.plugin.path(),
      self.proto.path(),
    ]

  @property
  def protoc_cc_cmd(self):
    return [
      self.protoc,
      '-I', self.proto.path().dirname(),
      '--cpp_out=%s' % self.output_dir,
      self.proto.path(),
    ]

  def hash(self):
    return {
      'grpc-cmd': list(map(str, self.protoc_grpc_cmd)),
      'protoc-cmd': list(map(str, self.protoc_cc_cmd)),
    }

class PyGen(Generator):
  '''Generate the python protobuf serialization and gRPC files from a .proto.

  Resulting files for <xxx.proto> will be:
  - xxx_pb2.py
  - xxx_pb2_grpc.py
  '''

  def __init__(self, proto, protoc = None, plugin = None):
    name = proto.name_relative.without_last_extension()
    dsts = drake.nodes(
      '{}_pb2.py'.format(name),
      '{}_pb2_grpc.py'.format(name),
    )
    if plugin is None:
      plugin = drake.node('/usr/local/bin/grpc_python_plugin')
    super().__init__(proto, protoc = protoc, plugin = plugin, targets = dsts)

  def execute(self):
    self.cmd('GRPC protoc python %s' % self.proto, self.command, throw = True)
    return True

  @property
  def command(self):
    return [
      self.protoc,
      '-I', self.proto.path().dirname(),
      '--python_out=%s' % self.output_dir,
      '--grpc_python_out=%s' % self.output_dir,
      '--plugin=protoc-gen-grpc_python=%s' % self.plugin.path(),
      self.proto.path(),
    ]

  def hash(self):
    return list(map(str, self.command))

class GoGen(Generator):
  '''Generate the Go protobuf serialization and gRPC files from a .proto.

  Resulting files for <xxx.proto> will be:
  - xxx.pb.go
  '''

  def __init__(self, proto, protoc = None, plugin = None, toolkit = None):
    name = proto.name_relative.without_last_extension()
    dsts = drake.nodes(
      name.with_extension('pb.go'),
    )
    srcs = []
    if plugin is None:
      if toolkit is not None:
        if toolkit.os == 'windows':
          plugin = drake.node('%s/bin/%s/protoc-gen-go.exe' % (
            toolkit.path, toolkit.platform_str()))
        else:
          plugin = drake.node('%s/bin/protoc-gen-go' % toolkit.path)
        if not plugin.builder:
          drake.go.FetchPackage(
            url = 'github.com/golang/protobuf/protoc-gen-go',
            toolkit = toolkit,
            targets = [
              plugin,
            ],
          )
      elif os.environ.get('GOPATH'):
        plugin = drake.node('%s/bin/protoc-gen-go' % os.environ['GOPATH'])
      elif os.environ.get('GOROOT'):
        plugin = drake.node('%s/bin/protoc-gen-go' % os.environ['GOROOT'])
    self.__toolkit = toolkit
    super().__init__(proto, protoc = protoc, plugin = plugin, targets = dsts)

  def execute(self):
    self.cmd('GRPC protoc Go %s' % self.proto, self.command, throw = True)
    return True

  @property
  def command(self):
    return [
      self.protoc,
      '-I', self.proto.path().dirname(),
      '--go_out=plugins=grpc:%s' % self.output_dir,
      '--plugin=protoc-gen-go=%s' % self.plugin.path(),
      self.proto.path(),
    ]

  def hash(self):
    return list(map(str, self.command))
