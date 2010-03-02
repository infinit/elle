import drake

drake.run = False

class FooBuilder(drake.Builder):

  name = 'foo'

  def __init__(self, dst):

    drake.Builder.__init__(self, [], [dst])
    self.dst = dst

  def execute(self):

    f = open(str(self.dst), 'w')
    print >> f, 'foo'
    f.close()
    drake.run = True
    return True

def configure():

  tgt = drake.Node('tgt')
  FooBuilder(tgt)
