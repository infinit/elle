import drake
import os
import shutil
import tempfile

class Drake:

  def __init__(self, dir = None, *args, **kwargs):
    self.__dir = dir
    self.__delete = False
    self.__drake = drake.Drake(*args, **kwargs)

  def __enter__(self):
    if self.__dir is None:
      self.__dir = tempfile.mkdtemp()
      self.__delete = True
    os.chdir(self.__dir)
    self.__drake.__enter__()
    return self.__drake

  def __exit__(self, *args):
    self.__drake.__exit__(*args)
    if self.__delete:
      shutil.rmtree(self.__dir)


class FailBuilder(drake.Builder):

  def execute(self):
    return False


class TouchBuilder(drake.Builder):

  def execute(self):
    for node in self.targets():
      node.path().touch()
    return True


class BeaconException(Exception):
  pass

def assertEq(a, b):
  if a != b:
    raise Exception('%r != %r' % (a, b))

def assertExcept(f):
  try:
    f()
  except:
    pass
  else:
    raise Exception('%s did not except', f)

def assertNotEq(a, b):
  if a == b:
    raise Exception('%r == %r' % (a, b))

def assertGt(a, b):
  if a <= b:
    raise Exception('%r <= %r' % (a, b))

def assertGe(a, b):
  if a < b:
    raise Exception('%r < %r' % (a, b))

def assertNotGt(a, b):
  if a > b:
    raise Exception('%r > %r' % (a, b))

def assertNotGe(a, b):
  if a >= b:
    raise Exception('%r >= %r' % (a, b))

def assertLt(a, b):
  if a >= b:
    raise Exception('%r >= %r' % (a, b))

def assertLe(a, b):
  if a > b:
    raise Exception('%r > %r' % (a, b))

def assertNotLt(a, b):
  if a < b:
    raise Exception('%r < %r' % (a, b))

def assertNotLe(a, b):
  if a <= b:
    raise Exception('%r <= %r' % (a, b))

def assertIn(v, c):
  if not v in c:
    raise Exception('%s not in %s' % (v, c))

def assertNotIn(v, c):
  if v in c:
    raise Exception('%s in %s' % (v, c))
