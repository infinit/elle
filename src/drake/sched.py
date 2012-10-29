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

class Terminate(Exception):
  pass

class OrderedSet:

  def __init__(self):
    self.__set = set()
    self.__list = []

  def add(self, v):
    self.__set.add(v)
    self.__list.append(v)

  def remove(self, v):
    self.__set.remove(v)

  def __iter__(self):
    remove = []
    i = 0
    while i < len(self.__list):
      elt = self.__list[i]
      if elt in self.__set:
        yield elt
        i += 1
      else:
        del self.__list[i]

  def __bool__(self):
    return bool(self.__set)

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
    self.__coroutines = OrderedSet()
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
    self.__coroutines.add(coro)

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

        running = list(self.__coroutines)
        for coro in running:
          self.__step(coro)
    finally:
      Scheduler.__pool.stop()
      Scheduler.__pool = None
      self.__running = False

  def __step(self, coro):
    if (coro.done):
      return
    self.debug('step %s' % coro)
    triggered = None
    try:
      coro.step()
    except Exception as e:
      parent = coro._Coroutine__parent
      if parent is not None:
        parent.throw(e, coro._Coroutine__traceback)
        triggered = parent
      else:
        self.__exception = e.with_traceback(coro._Coroutine__traceback)
    if coro.done:
      self.debug('%s ended' % coro)
      self.__coroutines.remove(coro)
    elif coro.frozen:
      self.debug('%s froze' % coro)
      self.__coroutines_frozen.add(coro)
      self.__coroutines.remove(coro)
    if triggered is not None:
      self.__step(triggered)

  def unfreeze(self, coro):
    self.debug('%s wakes up' % coro)
    self.__coroutines.add(coro)
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

  def __unwait(self, coro):
    self.__waiting.remove(coro)

  def __wake_all(self):
    for coro in self.__waiting:
      coro._Coroutine__unwait(self)
    self.__waiting.clear()

  def __wake_one(self):
    if self.__waiting:
      self.__wake(next(iter(self.__waiting)))

  def __wake(self, coro):
    coro._Coroutine__unwait(self)
    self.__waiting.remove(coro)

class Signal(Waitable):

  def signal(self):
    self._Waitable__wake_all()

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

  def throw(self, exn, tb = None):
    self.__exception = exn
    self.__traceback = tb
    if self.frozen:
      self.__unfreeze()
      for waited in self.__waited:
        waited._Waitable__unwait(self)
      self.__waited.clear()

  @property
  def parent(self):
    return self.__parent

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

  def __repr__(self):
    return 'Coroutine(%s)' % self.name

  def __yield(self, *args, **kwargs):
    self.__coro.parent.switch(*args, **kwargs)

  def wait(self, waitables, handle_exceptions = True):
    if isinstance(waitables, Waitable):
      return self.wait([waitables], handle_exceptions)
    else:
      freeze = False
      for waitable in waitables:
        if waitable._Waitable__wait(self):
          self.__waited.add(waitable)
          freeze = True
      if freeze:
        self.__frozen = True
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
    except Terminate:
      assert not self.__coro
      self.__done_set()
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
    self._Waitable__wake_all()

  def __unfreeze(self):
    assert self.frozen
    self.__frozen = False
    if self.__scheduler:
      self.__scheduler.unfreeze(self)

  def terminate(self):
    if self.done:
      return
    if self.current:
      raise Terminate()
    else:
      self.throw(Terminate())
      self.step()

class ThreadedOperation(Signal):

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
    raise exception.with_traceback(Coroutine.current._Coroutine__traceback)

def background(f):
  class Background(ThreadedOperation):
    def run(self):
      self.result = f()
  op = Background()
  op.start()
  coro_wait(op)
  return op.result

class Lockable:

  def lock(self):
    return self.__lock()

  def unlock(self):
    return self.__unlock()

  def __enter__(self):
    return self.lock()

  def __exit__(self, t, value, tb):
    self.unlock()

class Semaphore(Waitable, Lockable):

  def __init__(self, count = 0):
    Waitable.__init__(self)
    Lockable.__init__(self)
    self.__count = count

  @property
  def count(self):
    return self.__count

  def __str__(self):
    return 'Semaphore %s' % id(self)

  def _Waitable__wait(self, coro):
    if self.count == 0:
      drake.debug.debug('%s: wait' % self)
      return Waitable._Waitable__wait(self, coro)
    else:
      drake.debug.debug('%s: lock one' % self)
      self.__count = self.__count - 1
      return False

  def _Lockable__lock(self, coro = None):
    coro = coro or Coroutine.current
    return coro.wait(self)

  def _Lockable__unlock(self):
    if self.__count == 0:
      Waitable._Waitable__wake_one(self)
      return True
    else:
      self.__count += 1
      return False
