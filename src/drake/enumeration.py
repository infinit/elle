# Copyright (C) 2013, Quentin "mefyl" Hocquet
#
# This software is provided "as is" without warranty of any kind,
# either expressed or implied, including but not limited to the
# implied warranties of fitness for a particular purpose.
#
# See the LICENSE file for more information.

class Enumeration(type):

  def __new__(self, name, bases, dct,
              values = [], orderable = False):
    return super(Enumeration, self).__new__(self, name, bases, dct)

  def __init__(self, name, bases, dct,
               values = [], orderable = False):
    super(Enumeration, self).__init__(name, bases, dct)
    self.__instances = {}
    self.__orderable = orderable
    for value in values:
      self(name = value)

  def __register(self, instance):
    if self.__orderable:
      instance.index = len(self.__instances)
    self.__instances[instance.name] = instance
    setattr(self, instance.name, instance)

  def __iter__(self):
    return iter(self.__instances.values())

class Enumerated(metaclass = Enumeration):

  def __init__(self, name):
    self.__name = name
    self.__class__._Enumeration__register(self)

  def __str__(self):
    return '%s.%s' % (self.__class__.__name__, self.name)

  @property
  def name(self):
    return self.__name

  def __eq__(self, other):
    return self is other

  def __lt__(self, other):
    return self.index < other.index

  def __le__(self, other):
    return self.index <= other.index
