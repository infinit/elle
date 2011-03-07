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

class Frozen:
  pass

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

  def current_job_id(self):
    return self.__local.i

  def running(self):
    return self.__running

  def add(self, coro):
    debug.debug('new coroutine: %s' % (coro.name), debug.DEBUG_SCHED)
    with self.__sem:
      self.__coroutines.append(coro)
      self.ncoro += 1
      self.waiting_coro_lock.release()
    debug.debug('new coroutine: released.', debug.DEBUG_SCHED)

  def freeze(self, frozen, blocker):
    with self.__sem:
      if not blocker.done():
        blocker._Coroutine__waiters.append(frozen)
        return True
      else:
        return False

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
              debug.debug('no more coroutine, dying', debug.DEBUG_SCHED)
            else:
              debug.debug('pending exception %s, dying' % (self.__exception,),
                          debug.DEBUG_SCHED)
            # Tell all jobs they must die
            self.die = True
            # Wake everyone
            debug.debug('wake up everyone.', debug.DEBUG_SCHED)
            for i in range(self.__jobs - 1):
              self.waiting_coro_lock.release()
            # Quit
            debug.debug('bye bye.', debug.DEBUG_SCHED)
            return
        # Lock one coroutine slot
        if not self.waiting_coro_lock.acquire(False):
          debug.debug('no more coroutine available, sleeping', debug.DEBUG_SCHED)
          self.waiting_coro_lock.acquire()
          debug.debug('woken up', debug.DEBUG_SCHED)
        # If we must die, do so
        if self.die:
          debug.debug('scheduler is dying, dying too', debug.DEBUG_SCHED)
          return
        # Fetch our coroutine
        with self.__sem:
          coro = self.__coroutines[-1]
          del self.__coroutines[-1]
        res = None
        try:
          # Run one step of our coroutine
          self.__local.coroutine = coro
          debug.debug('step %s' % (coro.name), debug.DEBUG_SCHED)
          res = coro.step()
          self.__local.coroutine = None
        except:
          self.__local.coroutine = None
          self.__exception = sys.exc_info()
          debug.debug('caught exception %s' % (self.__exception[1]), debug.DEBUG_SCHED)
        with self.__sem:
          if res:
            if res is Frozen:
              debug.debug('coroutine froze: %s' % (coro.name), debug.DEBUG_SCHED)
            else:
              debug.debug('pushing coroutine back: %s' % (coro.name), debug.DEBUG_SCHED)
              self.__coroutines.append(coro)
              self.waiting_coro_lock.release()
          else:
            debug.debug('coroutine ended: %s' % (coro.name), debug.DEBUG_SCHED)
            self.ncoro -= 1
            for coro in coro._Coroutine__waiters:
              debug.debug('coroutine waking up: %s' % (coro.name), debug.DEBUG_SCHED)
              self.__coroutines.append(coro)
              self.waiting_coro_lock.release()

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
    self.__waiters = []
    self.__scheduler = scheduler
    if scheduler is not None:
      scheduler.add(self)

  def __str__(self):
    return 'coro %s' % self.name

  def run(self):
    while self.routine:
      self.__step()

  def step(self):
    while True:
      res = self.__step()
      if not res:
        break
      if res is Frozen:
        return Frozen
      if not self.routine:
        self.__done = True
        return False
    return True

  def done(self):
    return self.__done

  def __step(self):
    try:
      value = self.routine[-1].next()
      if isinstance(value, types.GeneratorType):
        self.routine.append(value)
        return True
      if isinstance(value, Coroutine) and not value.done():
        if self.__scheduler is not None:
          if self.__scheduler.freeze(self, value):
            return Frozen
          else:
            return True
        else:
          print self
          print value
          assert False
      else:
        return False
    except StopIteration:
      del self.routine[-1]
      return True
