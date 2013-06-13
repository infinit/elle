# Copyright (C) 2013, Quentin "mefyl" Hocquet
#
# This software is provided "as is" without warranty of any kind,
# either expressed or implied, including but not limited to the
# implied warranties of fitness for a particular purpose.
#
# See the LICENSE file for more information.

class Enumeration(type):

  def __new__(self, name, bases, dct, values = []):
    return super(Enumeration, self).__new__(self, name, bases, dct)

  def __init__(self, name, bases, dct, values = []):
    self.__instances = {}
    for value in values:
      self(name = value)
    super(Enumeration, self).__init__(name, bases, dct)

  def __register(self, instance):
    self.__instances[instance.name] = instance

  def __iter__(self):
    return iter(self.__instances.values())

  def __getattr__(self, name):
    if name in self.__instances:
      return self.__instances[name]
    else:
      return super(Enumeration, self).__getattr__(self, name)

class Enumerated(metaclass = Enumeration):

  def __init__(self, name):
    self.__name = name
    self.__class__._Enumeration__register(self)

  def __str__(self):
    return '%s.%s' % (self.__class__.__name__, self.name)

  @property
  def name(self):
    return self.__name
