# Copyright (C) 2011, Quentin "mefyl" Hocquet
#
# This software is provided "as is" without warranty of any kind,
# either expressed or implied, including but not limited to the
# implied warranties of fitness for a particular purpose.
#
# See the LICENSE file for more information.

import debug
import sys
import threading
import types

class Scheduler:

  def __init__(self, jobs = 1):
    global _SCHEDULER
    self.__coroutines = []
    self.waiting_coro_lock = threading.Semaphore(0)
    self.ncoro = 0
    self.__jobs = jobs
    self.__running = False
    self.__exception = None
    self.__sem = threading.Semaphore(1)
    self.__local = threading.local()
    self.__local.i = -1
    self.__local.coroutine = None
    _SCHEDULER = self

  def running(self):
    return self.__running

  def add(self, coro):
    with self.__sem:
      debug.debug('%s: new coroutine: %s' % (self.__local.i, coro.name), debug.DEBUG_SCHED)
    self.__coroutines.append(coro)
    self.ncoro += 1
    self.waiting_coro_lock.release()

  def woken_up(self, coro):
    with self.__sem:
      debug.debug('%s: coroutine woke up: %s' % (self.__local.i, coro.name), debug.DEBUG_SCHED)
    self.__coroutines.append(coro)
    self.waiting_coro_lock.release()

  def run(self):
    self.__running = True
    self.die = False

    def job(i):
      self.__local.i = i
      while True:
        # If there are no more coroutines
        with self.__sem:
            if self.ncoro == 0 or self.__exception is not None:
              if self.ncoro == 0:
                debug.debug('%s: no more coroutine, dying' % self.__local.i, debug.DEBUG_SCHED)
              else:
                debug.debug('%s: pending exception, dying' % self.__local.i, debug.DEBUG_SCHED)
              # Tell all jobs they must die
              self.die = True
              # Wake everyone
              for i in range(self.__jobs - 1):
                self.waiting_coro_lock.release()
              # Quit
              break
        # Lock one coroutine slot
        if not self.waiting_coro_lock.acquire(False):
          debug.debug('%s: no more coroutine available, sleeping' % self.__local.i, debug.DEBUG_SCHED)
          self.waiting_coro_lock.acquire()
          debug.debug('%s: woken up' % self.__local.i, debug.DEBUG_SCHED)
        # If we must die, do so
        if self.die:
          with self.__sem:
            debug.debug('%s: scheduler is dying, dying too' % self.__local.i, debug.DEBUG_SCHED)
          return
        # Fetch our coroutine
        with self.__sem:
          coro = self.__coroutines[-1]
          del self.__coroutines[-1]
        res = None
        try:
          # Run one step of our coroutine
          with self.__sem:
            debug.debug('%s: step %s' % (self.__local.i, coro.name), debug.DEBUG_SCHED)
          self.__local.coroutine = coro
          res = coro.step()
          self.__local.coroutine = None
        except:
          self.__exception = sys.exc_info()
        with self.__sem:
          if res:
            if coro.frozen():
              debug.debug('%s: coroutine froze: %s' % (self.__local.i, coro.name), debug.DEBUG_SCHED)
            else:
              debug.debug('%s: pushing coroutine back: %s' % (self.__local.i, coro.name), debug.DEBUG_SCHED)
              self.__coroutines.append(coro)
              self.waiting_coro_lock.release()
          else:
            debug.debug('%s: coroutine ended: %s' % (self.__local.i, coro.name), debug.DEBUG_SCHED)
            self.ncoro -= 1
    if self.__jobs == 1:
      job(0)
    else:
      threads = [threading.Thread(target = job, args=[i]) for i in range(self.__jobs)]
      for thread in threads:
        thread.start()
      for thread in threads:
        thread.join()
    self.__running = False
    if self.__exception is not None:
      raise self.__exception[1], None, self.__exception[2]

  def coroutine(self):

    res = self.__local.coroutine
    assert res
    return res

class Coroutine:

  def __init__(self, routine, name, scheduler):
    self.routine = [routine]
    self.name = name
    self.__done = False
    self.__frozen = False
    self.__waiters = []
    self.__scheduler = scheduler
    scheduler.add(self)

  def frozen(self):
    return self.__frozen

  def __str__(self):
    return 'coro %s' % self.name

  def run(self):
    while self.routine:
      self._step()

  def step(self):
    if not self.__frozen:
      while self._step():
        if not self.routine:
          self.__done = True
          for coro in self.__waiters:
            coro.__frozen = False
            self.__scheduler.woken_up(coro)
          return False
    return True

  def done(self):
    return self.__done

  def _step(self):
    try:
      value = self.routine[-1].next()
      if isinstance(value, types.GeneratorType):
        self.routine.append(value)
        return True
      if isinstance(value, Coroutine) and not value.done():
        self.__frozen = True
        value.__waiters.append(self)
        return False
      else:
        return False
    except StopIteration:
      del self.routine[-1]
      return True
