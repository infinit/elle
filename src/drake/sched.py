# Copyright (C) 2011, Quentin "mefyl" Hocquet
#
# This software is provided "as is" without warranty of any kind,
# either expressed or implied, including but not limited to the
# implied warranties of fitness for a particular purpose.
#
# See the LICENSE file for more information.

import greenlet
import sys
import threading
import time
import traceback
import types
import drake.debug
import drake.threadpool

class Frozen:
  pass

class CoroutineDone(Exception):
  pass

class CoroutineFrozen(Exception):
  pass

class Scheduler:

  __instance = None
  __pool = None

  @classmethod
  def scheduler(self):
    return Scheduler.__instance

  def __init__(self):
    self.reset()
    Scheduler.__instance = self
    self.__lock = threading.Condition()
    self.__scheduled = []

  def reset(self):
    self.__coroutines = []
    self.__coroutines_frozen = set()
    self.__running = False
    self.__exception = None
    self.__traceback = None

  def running(self):
    return self.__running

  def debug(self, msg):
    drake.debug.debug(msg, drake.debug.DEBUG_SCHED)


  def add(self, coro):
    self.debug('new coroutine: %s' % (coro.name))
    self.__coroutines.append(coro)

  def run(self):

    assert not self.__running

    self.__running = True
    self.die = False
    Scheduler.__pool = drake.threadpool.ThreadPool()

    try:
      while True:
        if self.__exception is not None:
          self.debug('pending exception %s, dying' % (self.__exception,))
          # FIXME: terminate everyone
          e = self.__exception
          self.__exception = None
          raise e
        if self.__coroutines:
          with self.__lock:
            for f in self.__scheduled:
              f()
            self.__scheduled = []
        elif not self.__coroutines_frozen:
          self.debug('no more coroutine, dying')
          break
        else:
          while not self.__coroutines:
            with self.__lock:
              if not self.__scheduled:
                self.__lock.wait()
              assert self.__scheduled
              for f in self.__scheduled:
                f()
              self.__scheduled = []

        i = 0
        while i < len(self.__coroutines):
          coro = self.__coroutines[i]
          self.debug('step %s' % coro)
          try:
            coro.step()
          except Exception as e:
            parent = coro._Coroutine__parent
            if parent is not None:
              # FIXME: wake it up ?
              parent._Coroutine__exception = e
              parent._Coroutine__traceback = coro._Coroutine__traceback
            else:
              self.__exception = e.with_traceback(coro._Coroutine__traceback)
          if coro.done:
            self.debug('%s ended' % coro)
            del self.__coroutines[i]
          elif coro.frozen:
            self.debug('%s froze' % coro)
            self.__coroutines_frozen.add(coro)
            del self.__coroutines[i]
          else:
            i += 1
    finally:
      Scheduler.__pool.stop()
      Scheduler.__pool = None
      self.__running = False

  def unfreeze(self, coro):
    self.debug('%s wakes up' % coro)
    self.__coroutines.append(coro)
    self.__coroutines_frozen.remove(coro)

  def run_one(self, coro):
    assert not self.running()
    self.reset()
    try:
      prev = self.__local.coroutine
      self.__local.coroutine = coro
      while coro.routine:
        coro._Coroutine__step()
    finally:
      self.__local.coroutine = prev

  def schedule(self, f):
    with self.__lock:
      self.__scheduled.append(f)
      self.__lock.notify()

class Waitable:
  def __init__(self):
    self.__waiting = set()

  def __wait(self, coro):
    self.__waiting.add(coro)
    return True

  def signal(self):
    for coro in self.__waiting:
      coro._Coroutine__unwait(self)
    self.__waiting.clear()

class classproperty:
  def __init__(self, f):
    self.__f = f
  def __get__(self, instance, owner):
    return self.__f(instance or owner)

class Coroutine(Waitable):

  __current = None

  def __init__(self, routine, name, scheduler = None, parent = None):
    Waitable.__init__(self)
    self.__coro = greenlet.greenlet(routine)
    self.__done = False
    self.__done_hooks = []
    self.__exception = None
    self.__frozen = False
    self.__joined = False
    self.__name = name
    self.__parent = parent
    self.__scheduler = scheduler
    self.__traceback = None
    self.__waited = set()
    if scheduler is not None:
      scheduler.add(self)

  @property
  def name(self):
    return self.__name

  @property
  def frozen(self):
    """Whether this coroutine is frozen."""
    return self.__frozen

  @property
  def done(self):
    """Whether this coroutine is done."""
    return self.__done

  @classproperty
  def current(self):
    """If called on a coroutine, whether this coroutine is the current one.
    If called on Coroutine, the current coroutine."""
    if self is Coroutine:
      return Coroutine.__current
    else:
      return self is Coroutine.__current

  def _Waitable__wait(self, coro):
    if self.done:
      return False
    else:
      return Waitable._Waitable__wait(self, coro)

  def __str__(self):
    return 'coroutine %s' % self.name

  def __yield(self, *args, **kwargs):
    self.__coro.parent.switch(*args, **kwargs)

  def wait(self, waitable, handle_exceptions = True):
    if waitable._Waitable__wait(self):
      self.__frozen = True
      self.__waited.add(waitable)
      if self.current:
        coro_yield(handle_exceptions = handle_exceptions)


  def __unwait(self, waitable):
    assert waitable in self.__waited
    self.__waited.remove(waitable)
    if not self.__waited:
      self.__unfreeze()

  def run(self):
    while not self.done:
      self.step()

  def step(self):
    """Execute one step of the coroutine, until the next yield or freeze."""
    if self.done:
      raise CoroutineDone()
    if self.frozen:
      raise CoroutineFrozen()
    self.__coro.parent = greenlet.getcurrent()
    prev = Coroutine.__current
    try:
      Coroutine.__current = self
      self.__coro.switch()
    except Exception as e:
      self.__done_set()
      self.__traceback = e.__traceback__.tb_next
      raise
    else:
      if not self.__coro:
        self.__done_set()
    finally:
      Coroutine.__current = prev

  def __done_set(self):
    self.__done = True
    for c in self.__done_hooks:
      c()
    self.signal()

  def __unfreeze(self):
    assert self.__frozen
    self.__frozen = False
    if self.__scheduler:
      self.__scheduler.unfreeze(self)




class ThreadedOperation(Waitable):

  def start(self):
    Scheduler._Scheduler__pool.run(self.__run)

  def __run(self):
    self.run()
    Scheduler.scheduler().schedule(self.signal)

def coro_yield(handle_exceptions = True):
  Coroutine.current._Coroutine__yield()
  if handle_exceptions:
    exception = Coroutine.current._Coroutine__exception
    if exception is not None:
      Coroutine.current._Coroutine__exception = None
      raise exception.with_traceback(Coroutine.current._Coroutine__traceback)

def coro_wait(waitable):
  Coroutine.current.wait(waitable, handle_exceptions = False)
  exception = Coroutine.current._Coroutine__exception
  if exception is not None:
    Coroutine.current._Coroutine__exception = None
    import traceback
    raise exception.with_traceback(Coroutine.current._Coroutine__traceback)
