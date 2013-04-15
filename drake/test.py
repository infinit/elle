import drake

class TestRunner(drake.Builder):

  def __init__(self, test):
    self.__test = test
    self.__out = drake.node('%s.out' % test.name())
    self.__err = drake.node('%s.err' % test.name())
    self.__status = drake.node('%s.status' % test.name())
    drake.Builder.__init__(self, [test], [self.__out, self.__err, self.__status])

  @property
  def status(self):
    return self.__status

  def execute(self):
    import subprocess
    path = str(self.__test.path())
    with open(str(self.__out.path()), 'w') as out, \
         open(str(self.__err.path()), 'w') as err, \
         open(str(self.__status.path()), 'w') as rv:
      self.output(path, 'Test %s' % self.__test)
      p = subprocess.Popen(path, stdout = out, stderr = err)
      p.wait()
      status = p.returncode
      print(status, file = rv)
    return status == 0
