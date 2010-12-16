import drake

run = False

class FooBuilder(drake.Builder):

  name = 'foo'

  def __init__(self, dst):

    drake.Builder.__init__(self, [], [dst])
    self.dst = dst

  def execute(self):

    global run
    f = open(str(self.dst), 'w')
    print >> f, 'foo'
    f.close()
    run = True
    return True

def configure():

  tgt = drake.Node('tgt')
  FooBuilder(tgt)
