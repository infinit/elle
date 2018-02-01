import drake, os

run = False

class CopyBuilder(drake.Builder):

  def __init__(self, src, dst):

    drake.Builder.__init__(self, [src], [dst])
    self.src = src
    self.dst = dst

  def execute(self):

    global run
    os.system('cp %s %s' % (self.src, self.dst))
    run = True
    return True

def configure():

  src = drake.Node('src')
  dst = drake.Node('dst')
  CopyBuilder(src, dst)
