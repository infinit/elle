#!/usr/bin/env python3
# Copyright (C) 2011, Quentin "mefyl" Hocquet
#
# This software is provided "as is" without warranty of any kind,
# either expressed or implied, including but not limited to the
# implied warranties of fitness for a particular purpose.
#
# See the LICENSE file for more information.

import sys
import threading
import time
import traceback
import types
import drake.debug

class Frozen:
  pass

class CoroutineDone(Exception):
  pass

class CoroutineFrozen(Exception):
  pass

class Scheduler:

  def __init__(self, jobs = 1):
    self.__jobs = jobs
    self.reset()

  def reset(self):
    self.__coroutines = []
    self.waiting_coro_lock = threading.Semaphore(0)
    self.ncoro = 0
    self.__running = False
    self.__exception = None
    self.__traceback = None
    self.__sem = threading.Semaphore(1)
    self.__local = threading.local()
    self.__local.i = -1
    self.__local.coroutine = None

  def jobs(self):
    return self.__jobs

  def current_job_id(self):
    return self.__local.i

  def running(self):
    return self.__running

  def add(self, coro):
    drake.debug.debug('new coroutine: %s' % (coro.name), drake.debug.DEBUG_SCHED)
    with self.__sem:
      self.__coroutines.append(coro)
      self.ncoro += 1
      self.waiting_coro_lock.release()
    drake.debug.debug('new coroutine: released.', drake.debug.DEBUG_SCHED)

  def run(self):
    self.__running = True
    self.die = False

    def job(i):
      self.__local.i = i
      drake.debug.debug('created.', drake.debug.DEBUG_SCHED)
      while True:
        # If there are no more coroutines
        with self.__sem:
          if self.ncoro == 0 or self.__exception is not None:
            if self.ncoro == 0:
              drake.debug.debug('no more coroutine, dying', drake.debug.DEBUG_SCHED)
            else:
              drake.debug.debug('pending exception %s, dying' % (self.__exception,),
                          drake.debug.DEBUG_SCHED)
            # Tell all jobs they must die
            self.die = True
            # Wake everyone
            drake.debug.debug('wake up everyone.', drake.debug.DEBUG_SCHED)
            for i in range(self.__jobs - 1):
              self.waiting_coro_lock.release()
            # Quit
            drake.debug.debug('bye bye.', drake.debug.DEBUG_SCHED)
            return
        # Lock one coroutine slot
        if not self.waiting_coro_lock.acquire(False):
          drake.debug.debug('no more coroutine available, sleeping', drake.debug.DEBUG_SCHED)
          self.waiting_coro_lock.acquire()
          drake.debug.debug('woken up', drake.debug.DEBUG_SCHED)
        # If we must die, do so
        if self.die:
          drake.debug.debug('scheduler is dying, dying too', drake.debug.DEBUG_SCHED)
          return
        # Fetch our coroutine
        with self.__sem:
          coro = self.__coroutines[-1]
          del self.__coroutines[-1]
        res = None
        try:
          # Run one step of our coroutine
          self.__local.coroutine = coro
          drake.debug.debug('step %s' % (coro.name), drake.debug.DEBUG_SCHED)
          res = coro.step()
        except:
          self.__exception = sys.exc_info()
          drake.debug.debug('caught exception %s' % (self.__exception[1]), drake.debug.DEBUG_SCHED)
        finally:
          self.__local.coroutine = None
        with self.__sem:
          if res:
            if res is Frozen:
              drake.debug.debug('coroutine froze: %s' % (coro.name), drake.debug.DEBUG_SCHED)
              coro.hook_unfrozen(lambda c: self.unfreeze(c))
            else:
              drake.debug.debug('pushing coroutine back: %s' % (coro.name), drake.debug.DEBUG_SCHED)
              self.__coroutines.append(coro)
              self.waiting_coro_lock.release()
          else:
            drake.debug.debug('coroutine ended: %s' % (coro.name), drake.debug.DEBUG_SCHED)
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
      exn = self.__exception
      self.__exception = None
      raise exn[1]

  def unfreeze(self, coro):
    drake.debug.debug('coroutine waking up: %s' % (coro.name), drake.debug.DEBUG_SCHED)
    self.__coroutines.append(coro)
    self.waiting_coro_lock.release()

  def coroutine(self):

    res = self.__local.coroutine
    assert res
    return res

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


class Coroutine:

  def __init__(self, routine, name, scheduler = None, parent = None):
    self.__routine = [routine]
    self.name = name
    self.__done = False
    self.__exception = None
    self.__traceback = None
    self.__scheduler = scheduler
    self.__joined = False
    self.__waited = None
    self.__parent = parent
    self.__done_hooks = []
    self.__unfrozen_hooks = []
    self.__lock_status = threading.Semaphore(1)
    if scheduler is not None:
      scheduler.add(self)

  def __str__(self):
    return 'coro %s' % self.name

  def hook_done(self, c):
    self.__done_hooks.append(c)

  def hook_unfrozen(self, c):
    self.__unfrozen_hooks.append(c)

  def run(self):
    while not self.done():
      self.step()

  def step(self):
    """Execute one step of the coroutine, until the next yield or freeze."""
    if self.__done:
      raise CoroutineDone()
    if self.__waited is not None:
      raise CoroutineFrozen()
    while True:
      res = self.__step()
      if not res:
        return True
      if res is Frozen:
        return Frozen
      if len(self.__routine) == 0:
        self.__done_set()
        return False

  def __done_set(self):
    with self.__lock_status:
      self.__done = True
      for c in self.__done_hooks:
        c()

  def frozen(self):
    """Whether this coroutine is frozen."""
    return self.__waited is not None

  def done(self):
    """Whether this coroutine is done."""
    return self.__done

  def __step(self):
    """Execute one internal step, until the next yield, freeze or coroutine recursion."""
    try:
      try:
        e = self.__exception
        self.__exception = None
        tb = self.__traceback
        self.__traceback = None
        value = self.__routine[-1].send(e and (e, tb))
      except StopIteration:
        raise
      except Exception as e:
        self.__exception = e
        self.__traceback = sys.exc_info()[2]
        if len(self.__routine) > 1:
          drake.debug.debug('caught exception %s, forwarding to the upper frame.' % e)
          del self.__routine[-1]
          return True
        else:
          if self.__parent is not None:
            drake.debug.debug('caught exception %s, forwarding to parent coroutine %s.' % (e, self.__parent))
            self.__parent.__exception = self.__exception
            self.__parent.__traceback = self.__traceback
            return True
          else:
            drake.debug.debug('caught exception %s, forwarding to the scheduler.' % e)
            self.__done_set()
            raise
      if isinstance(value, types.GeneratorType):
        self.__routine.append(value)
        return True
      if isinstance(value, Coroutine):
        with value.__lock_status:
          if not value.done():
            self.__waited = value
            value.hook_done(self.__unfreeze)
            return Frozen
          else:
            return True
      else:
        return False
    except StopIteration:
      del self.__routine[-1]
      return True

  def __unfreeze(self):
    self.__waited = None
    for c in self.__unfrozen_hooks:
      c(self)


def coro_recurse(res):
  if res is not None:
    raise res[0].with_traceback(res[1])

import unittest

class BeaconException(Exception):
  pass

class TestStandaloneCoroutine(unittest.TestCase):

  def setUp(self):
    self.beacon = 0

  def coroutine(self, n):
    self.beacon = n
    yield
    self.beacon = n + 1

  def coroutine_meta(self, n):
    self.beacon = n
    yield
    yield self.coroutine(n + 1)
    yield
    self.beacon = n + 3

  def coroutine_wait(self, coro, n):
    yield coro
    self.beacon = n

  def coroutine_raise(self):
    self.function_raise()
    yield

  def function_raise(self):
    raise BeaconException('exn')

  def coroutine_raise_meta(self):
    yield
    coro_recurse((yield self.coroutine_raise()))
    yield

  def test_one(self):
    self.c = Coroutine(self.coroutine(1), 'coro')
    self.assertFalse(self.c.done())
    self.assertEqual(self.beacon, 0)
    self.c.step()
    self.assertFalse(self.c.done())
    self.assertEqual(self.beacon, 1)
    self.c.step()
    self.assertTrue(self.c.done())
    self.assertEqual(self.beacon, 2)
    with self.assertRaises(CoroutineDone):
      self.c.step()

  def test_two(self):
    self.c1 = Coroutine(self.coroutine(1), 'coro1')
    self.c2 = Coroutine(self.coroutine(2), 'coro2')
    self.assertEqual(self.beacon, 0)
    self.c1.step()
    self.assertEqual(self.beacon, 1)
    self.c2.step()
    self.assertEqual(self.beacon, 2)
    self.c2.step()
    self.assertEqual(self.beacon, 3)
    self.c1.step()
    self.assertEqual(self.beacon, 2)
    with self.assertRaises(CoroutineDone):
      self.c1.step()
    with self.assertRaises(CoroutineDone):
      self.c2.step()

  def test_run(self):
    self.c = Coroutine(self.coroutine(1), 'coro')
    self.assertFalse(self.c.done())
    self.assertEqual(self.beacon, 0)
    self.c.run()
    self.assertTrue(self.c.done())
    self.assertEqual(self.beacon, 2)
    with self.assertRaises(CoroutineDone):
      self.c.step()

  def test_recursive(self):
    self.c = Coroutine(self.coroutine_meta(1), 'coro')
    self.assertEqual(self.beacon, 0)
    self.c.step()
    self.assertEqual(self.beacon, 1)
    self.c.step()
    self.assertEqual(self.beacon, 2)
    self.c.step()
    self.assertEqual(self.beacon, 3)
    self.c.step()
    self.assertEqual(self.beacon, 4)
    with self.assertRaises(CoroutineDone):
      self.c.step()

  def test_wait(self):
    self.c = Coroutine(self.coroutine(1), 'coro')
    self.w = Coroutine(self.coroutine_wait(self.c, 3), 'coro')
    self.assertFalse(self.c.done())
    self.assertFalse(self.w.done())
    self.assertFalse(self.w.frozen())
    self.assertTrue(self.w.step() is Frozen)
    self.assertTrue(self.w.frozen())
    with self.assertRaises(CoroutineFrozen):
      self.w.step()
    self.assertEqual(self.beacon, 0)
    self.c.step()
    self.assertEqual(self.beacon, 1)
    self.assertTrue(self.w.frozen())
    with self.assertRaises(CoroutineFrozen):
      self.w.step()
    self.c.step()
    self.assertEqual(self.beacon, 2)
    self.assertTrue(self.c.done())
    self.assertFalse(self.w.frozen())
    self.w.step()
    self.assertEqual(self.beacon, 3)
    self.assertTrue(self.w.done())

  def test_exception(self):
    self.r = Coroutine(self.coroutine_raise(), 'coro')
    with self.assertRaises(BeaconException):
      self.r.step()
    self.assertTrue(self.r.done())

  def test_exception_recurse(self):
    self.r = Coroutine(self.coroutine_raise_meta(), 'coro')
    self.r.step()
    with self.assertRaises(BeaconException):
      self.r.step()
    self.assertTrue(self.r.done())


class TestScheduler(unittest.TestCase):

  def __init__(self, *args, **kwargs):
    unittest.TestCase.__init__(self, *args, **kwargs)
    self.jobs = 1

  def setUp(self):
    self.scheduler = Scheduler(self.jobs)
    self.beacon1 = 0
    self.beacon2 = 0

  def coroutine1(self):
    self.beacon1 += 1
    yield
    self.beacon1 += 1

  def coroutine2(self):
    self.beacon2 += 1
    yield
    self.beacon2 += 1

  def coroutine_wait(self, coro):
    yield coro

  def test_basic(self):
    Coroutine(self.coroutine1(), 'coro', self.scheduler)
    self.assertEqual(self.beacon1, 0)
    self.scheduler.run()
    self.assertEqual(self.beacon1, 2)

  def test_several(self):
    Coroutine(self.coroutine1(), 'coro1', self.scheduler)
    Coroutine(self.coroutine2(), 'coro2', self.scheduler)
    self.assertEqual(self.beacon1, 0)
    self.assertEqual(self.beacon2, 0)
    self.scheduler.run()
    self.assertEqual(self.beacon1, 2)
    self.assertEqual(self.beacon2, 2)

  def test_wait(self):
    c1 = Coroutine(self.coroutine1(), 'coro1', self.scheduler)
    cw = Coroutine(self.coroutine_wait(c1), 'coro_wait', self.scheduler)
    self.scheduler.run()

class TestScheduler2(TestScheduler):

  def __init__(self, *args, **kwargs):
    TestScheduler.__init__(self, *args, **kwargs)
    self.jobs = 2


class TestScheduler3(TestScheduler):

  def __init__(self, *args, **kwargs):
    TestScheduler.__init__(self, *args, **kwargs)
    self.jobs = 3

if __name__ == '__main__':
  unittest.main()
