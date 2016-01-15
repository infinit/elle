import drake
import markdown2

class Renderer(drake.Builder):

  def __init__(self, source, target = None):
    self.__source = source
    self.__target = target or drake.Node(
      self.__source.path().with_extension('html')
    )
    drake.Builder.__init__(self, [self.__source], [self.__target])

  def execute(self):
    self.output('Render %s' % self.__target)
    with open(str(self.__source.path())) as input:
      with open(str(self.__target.path()), 'w') as output:
        output.write(
          markdown2.markdown(input.read(),
                             extras = ['header-ids'])
        )
    return True

class Source(drake.Node):

  def __init__(self, *args, **kwargs):
    super().__init__(*args, **kwargs)
    Renderer(self)

drake.Node.extensions['md'] = Source
