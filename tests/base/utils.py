import drake
import os
import shutil
import tempfile

class Drake:

  def __enter__(self):
    self.__dir = tempfile.mkdtemp()
    os.chdir(self.__dir)
    self.__drake = drake.Drake()
    self.__drake.__enter__()
    return drake

  def __exit__(self, *args):
    self.__drake.__exit__(*args)
    shutil.rmtree(self.__dir)
