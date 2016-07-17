# Copyright (C) 2011-2016, Quentin "mefyl" Hocquet
#
# This software is provided "as is" without warranty of any kind,
# either expressed or implied, including but not limited to the
# implied warranties of fitness for a particular purpose.
#
# See the LICENSE file for more information.

import collections
import greenlet
import os
import sys
import threading
import time
import traceback
import types

import drake.debug
import drake.threadpool
import drake.log


class Indentation:

  def __init__(self):
    self.__indentation = {}

  def __enter__(self):
    self.__indentation[Coroutine.current] += 1

  def __exit__(self, type, value, traceback):
    self.__indentation[Coroutine.current] -= 1

  @property
  def indentation(self):
    coroutine = Coroutine.current
    if coroutine not in self.__indentation:
      initial = 0
      if coroutine is not None and coroutine.parent is not None and coroutine.parent in self.__indentation:
        initial = self.__indentation[coroutine.parent]
      self.__indentation[coroutine] = initial
      return initial
    else:
      return self.__indentation[Coroutine.current]


conf = None
if 'DRAKE_LOG_LEVEL' in os.environ:
  conf = os.environ['DRAKE_LOG_LEVEL']
logger = drake.log.Logger(configuration_string = conf,
                          indentation = Indentation())

class Frozen:
  pass

class CoroutineDone(Exception):
  pass

class CoroutineFrozen(Exception):
  pass

class Terminate(Exception):

  def __init__(self, coroutine):
    self.__coroutine = coroutine

  def __str__(self):
    return 'termination of %s' % self.__coroutine


class Scope:

  __scopes = {}

  def __init__(self, exception_join = False):
    super().__init__()
    self.__exception_join = exception_join

  def __enter__(self):
    Scope.__scopes.setdefault(Coroutine.current, []).append(self)
    self.__coroutine = Coroutine.current
    self.__scheduler = self.__coroutine.scheduler
    self.__coroutines = []
    return self

  def __exit__(self, type, value, traceback):
    try:
      exception = None
      if value is not None:
        self.terminate()
      else:
        while True:
          try:
            self.__coroutine.wait(self.__coroutines)
            break
          except Exception as e:
            exception = e
            if self.__exception_join:
              continue
            else:
              self.terminate()
      if exception:
        raise exception
    finally:
      del Scope.__scopes[Coroutine.current][-1]

  def run(self, routine, name):
    coro = Coroutine(routine, name, self.__scheduler,
                     parent = self.__coroutine)
    self.__coroutines.append(coro)

  def terminate(self):
    for coro in self.__coroutines:
      coro.terminate()
    while True:
      try:
        self.__coroutine.wait(self.__coroutines)
      except:
        pass
      else:
        break

import oset
OrderedSet = oset.oset

class SchedulingPolicy:

  pass

class RoundRobin(SchedulingPolicy):

  def __init__(self):
    self.__coroutines = OrderedSet()

  @property
  def busy(self):
    return bool(self.__coroutines)

  def add(self, coroutine):
    self.__coroutines.add(coroutine)

  def remove(self, coroutine):
    self.__coroutines.remove(coroutine)

  def freeze(self, coroutine):
    self.__coroutines.remove(coroutine)

  def unfreeze(self, coroutine):
    self.__coroutines.add(coroutine)

  def round(self):
    for coro in list(self.__coroutines):
      yield coro


class DepthFirst(SchedulingPolicy):

  def __init__(self):
    self.__coroutines = OrderedSet()
    self.__hierarchy = {}

  @property
  def busy(self):
    return bool(self.__coroutines)

  def add(self, coroutine):
    parent = coroutine.parent
    self.__coroutines.add(coroutine)
    self.__hierarchy.setdefault(parent, OrderedSet()).add(coroutine)

  def remove(self, coroutine):
    self.__coroutines.remove(coroutine)
    self.__hierarchy.get(coroutine.parent).remove(coroutine)

  def freeze(self, coroutine):
    self.__coroutines.remove(coroutine)

  def unfreeze(self, coroutine):
    self.__coroutines.add(coroutine)

  def round(self):
    return (self.__round(self.__hierarchy.get(None, ())),)

  def __round(self, coroutines):
    for coroutine in coroutines:
      active = coroutine in self.__coroutines
      if active and coroutine.exception:
        return coroutine
      sub = self.__round(self.__hierarchy.get(coroutine, ()))
      if sub is not None:
        return sub
      if active:
        return coroutine

class Scheduler:

  __instance = None
  __pool = None

  @classmethod
  def scheduler(self):
    return Scheduler.__instance

  def __init__(self, jobs = None, policy = None):
    self.reset()
    Scheduler.__instance = self
    self.__lock = threading.Condition()
    self.__policy = policy or RoundRobin()
    self.__scheduled = []

  def __str__(self):
    return 'Scheduler'

  def reset(self):
    self.__coroutines_frozen = set()
    self.__running = False
    self.__exception = None
    self.__traceback = None

  def running(self):
    return self.__running

  def debug(self, msg):
    return logger.log('drake.scheduler', drake.log.LogLevel.trace, msg)

  def add(self, coro):
    self.debug('%s: new coroutine: %s' % (self, coro.name))
    self.__policy.add(coro)

  def run(self):

    assert not self.__running

    self.__running = True
    self.die = False
    Scheduler.__pool = drake.threadpool.ThreadPool()

    try:
      while True:
        if self.__exception is not None:
          self.debug('%s: pending exception %s, dying' % \
                     (self, self.__exception))
          # FIXME: terminate everyone
          e = self.__exception
          self.__exception = None
          raise e
        if not self.__policy.busy:
          if not self.__coroutines_frozen:
            self.debug('no more coroutine, dying')
            break
          else:
            while not self.__policy.busy:
              with self.__lock:
                if not self.__scheduled:
                  self.__lock.wait()
                assert self.__scheduled
                for f in self.__scheduled:
                  f()
                self.__scheduled = []
        for coro in self.__policy.round():
          assert coro is not None
          with self.__lock:
            for f in self.__scheduled:
              f()
            self.__scheduled = []
          self.__step(coro)
    finally:
      Scheduler.__pool.stop()
      Scheduler.__pool = None
      self.__running = False

  def __step(self, coro):
    if (coro.done):
      return
    assert not coro.frozen
    with self.debug('%s: schedule %s' % (self, coro)):
      try:
        with self.debug('%s: step %s' % (self, coro)):
          coro.step()
      except Exception as e:
        self.debug('%s: %s threw %s' % (self, coro, e))
        parent = coro._Coroutine__parent
        if parent is None:
          self.__exception = e#.with_traceback(coro._Coroutine__traceback)
      if coro.done:
        self.debug('%s ended' % coro)
        self.__policy.remove(coro)
      elif coro.frozen:
        self.debug('%s froze' % coro)
        self.__coroutines_frozen.add(coro)
        self.__policy.freeze(coro)

  def unfreeze(self, coro):
    self.debug('%s wakes up' % coro)
    self.__policy.unfreeze(coro)
    self.__coroutines_frozen.remove(coro)

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

  def __wake_all(self, exception = None):
    for coro in self.__waiting:
      coro._Coroutine__unwait(self, exception)
    self.__waiting.clear()

  def __wake_one(self):
    if self.__waiting:
      self.__wake(next(iter(self.__waiting)))
      return True
    else:
      return False

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
    self.__started = False
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
  def scheduler(self):
    return self.__scheduler

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

  @property
  def exception(self):
    return self.__exception

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
      with logger.log('drake.scheduler',
                      drake.log.LogLevel.trace,
                      '%s: wait %s', self, waitables):
        freeze = False
        for waitable in waitables:
          if waitable._Waitable__wait(self):
            self.__waited.add(waitable)
            freeze = True
        logger.log('drake.scheduler',
                   drake.log.LogLevel.debug,
                   '%s: block on %s', self, self.__waited)
        if freeze:
          self.__frozen = True
          if self.current:
            coro_yield(handle_exceptions = handle_exceptions)

  def __unwait(self, waitable, exception = None):
    assert waitable in self.__waited
    if exception is None:
      with logger.log('drake.scheduler',
                      drake.log.LogLevel.debug,
                      '%s: unwait %s', self, waitable):
        self.__waited.remove(waitable)
        if not self.__waited:
          self.__unfreeze()
    else:
      with logger.log(
          'drake.scheduler',
          drake.log.LogLevel.debug,
          '%s: wait on %s threw %s', self, waitable, exception):
        self.__exception = exception
        for w in self.__waited:
          if w is not waitable:
            assert self in w._Waitable__waiting
            w._Waitable__waiting.remove(self)
        self.__waited.clear()
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
    self.__started = True
    self.__coro.parent = greenlet.getcurrent()
    prev = Coroutine.__current
    try:
      Coroutine.__current = self
      self.__coro.switch()
    except Terminate:
      assert not self.__coro
      self.__done_set()
    except Exception as e:
      self.__done_set(e)
      self.__traceback = e.__traceback__.tb_next
      raise
    else:
      if not self.__coro:
        self.__done_set()
    finally:
      Coroutine.__current = prev

  def __done_set(self, e = None):
    self.__done = True
    for c in self.__done_hooks:
      c()
    self._Waitable__wake_all(e)

  def __unfreeze(self):
    assert self.frozen
    self.__frozen = False
    if self.__scheduler:
      self.__scheduler.unfreeze(self)

  def terminate(self):
    if self.done:
      return
    if not self.started:
      self.__done_set()
      self.scheduler._Scheduler__policy.remove(self)
    if self.current:
      raise Terminate(self)
    else:
      self.throw(Terminate(self))

  @property
  def status(self):
    if self.done:
      return 'done'
    elif self.frozen:
      return 'frozen'
    else:
      return 'running'

  @property
  def started(self):
    return self.__started

  @property
  def exception(self):
    return self.__exception

class ThreadedOperation(Signal):

  def start(self):
    Scheduler._Scheduler__pool.run(self.__run)

  def __run(self):
    try:
      self.run()
    finally:
      Scheduler.scheduler().schedule(self.signal)

def coro_yield(handle_exceptions = True):
  Coroutine.current._Coroutine__yield()
  if handle_exceptions:
    exception = Coroutine.current._Coroutine__exception
    if exception is not None:
      Coroutine.current._Coroutine__exception = None
      raise exception#.with_traceback(Coroutine.current._Coroutine__traceback)

def coro_wait(waitable):
  done = False
  exception = None
  while not done:
    Coroutine.current.wait(waitable, handle_exceptions = False)
    current_exception = Coroutine.current._Coroutine__exception
    Coroutine.current._Coroutine__exception = None
    if current_exception is None:
      done = True
    else:
      if exception is not None and exception is not current_exception:
        current_exception.__context__ = exception
      exception = current_exception
  if exception is not None:
    raise exception#.with_traceback(Coroutine.current._Coroutine__traceback)

def wait(waitable):
  return Coroutine.current.wait(waitable)

def background(f):
  class Background(ThreadedOperation):
    def run(self):
      try:
        self.result = f()
      except BaseException as e:
        self.exception = e
  op = Background()
  op.start()
  wait(op)
  if hasattr(op, 'exception'):
    raise op.exception
  else:
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
      if Waitable._Waitable__wake_one(self):
        return True
    self.__count += 1
    return False
