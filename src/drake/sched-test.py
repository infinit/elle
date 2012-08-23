#!/usr/bin/env python3

import sched
import unittest

class BeaconException(Exception):
  pass

class TestStandaloneCoroutine(unittest.TestCase):

  def setUp(self):
    self.beacon = 0

  def coroutine(self, n):
    self.beacon = n
    sched.coro_yield()
    self.beacon = n + 1

  def coroutine_meta(self, n):
    self.beacon = n
    sched.coro_yield()
    self.coroutine(n + 1)
    sched.coro_yield()
    self.beacon = n + 3

  def coroutine_wait(self, coro, n):
    sched.coro_wait(coro)
    self.beacon = n

  def coroutine_raise(self):
    self.function_raise()

  def function_raise(self):
    raise BeaconException('exn')

  def coroutine_raise_meta(self):
    sched.coro_yield()
    self.coroutine_raise()
    sched.coro_yield()

  def test_one(self):
    self.c = sched.Coroutine(lambda: self.coroutine(1), 'coro')
    self.assertFalse(self.c.done)
    self.assertEqual(self.beacon, 0)
    self.c.step()
    self.assertFalse(self.c.done)
    self.assertEqual(self.beacon, 1)
    self.c.step()
    self.assertTrue(self.c.done)
    self.assertEqual(self.beacon, 2)
    with self.assertRaises(sched.CoroutineDone):
      self.c.step()

  def test_two(self):
    self.c1 = sched.Coroutine(lambda: self.coroutine(1), 'coro1')
    self.c2 = sched.Coroutine(lambda: self.coroutine(2), 'coro2')
    self.assertEqual(self.beacon, 0)
    self.c1.step()
    self.assertEqual(self.beacon, 1)
    self.c2.step()
    self.assertEqual(self.beacon, 2)
    self.c2.step()
    self.assertEqual(self.beacon, 3)
    self.c1.step()
    self.assertEqual(self.beacon, 2)
    with self.assertRaises(sched.CoroutineDone):
      self.c1.step()
    with self.assertRaises(sched.CoroutineDone):
      self.c2.step()

  def test_run(self):
    self.c = sched.Coroutine(lambda: self.coroutine(1), 'coro')
    self.assertFalse(self.c.done)
    self.assertEqual(self.beacon, 0)
    self.c.run()
    self.assertTrue(self.c.done)
    self.assertEqual(self.beacon, 2)
    with self.assertRaises(sched.CoroutineDone):
      self.c.step()

  def test_recursive(self):
    self.c = sched.Coroutine(lambda: self.coroutine_meta(1), 'coro')
    self.assertEqual(self.beacon, 0)
    self.c.step()
    self.assertEqual(self.beacon, 1)
    self.c.step()
    self.assertEqual(self.beacon, 2)
    self.c.step()
    self.assertEqual(self.beacon, 3)
    self.c.step()
    self.assertEqual(self.beacon, 4)
    with self.assertRaises(sched.CoroutineDone):
      self.c.step()

  def test_wait(self):
    self.c = sched.Coroutine(lambda: self.coroutine(1), 'coro')
    self.w = sched.Coroutine(lambda: self.coroutine_wait(self.c, 3), 'coro')
    self.assertFalse(self.c.done)
    self.assertFalse(self.w.done)
    self.assertFalse(self.w.frozen)
    self.w.step()
    self.assertTrue(self.w.frozen)
    with self.assertRaises(sched.CoroutineFrozen):
      self.w.step()
    self.assertEqual(self.beacon, 0)
    self.c.step()
    self.assertEqual(self.beacon, 1)
    self.assertTrue(self.w.frozen)
    with self.assertRaises(sched.CoroutineFrozen):
      self.w.step()
    self.c.step()
    self.assertEqual(self.beacon, 2)
    self.assertTrue(self.c.done)
    self.assertFalse(self.w.frozen)
    self.w.step()
    self.assertEqual(self.beacon, 3)
    self.assertTrue(self.w.done)

  def test_exception(self):
    self.r = sched.Coroutine(self.coroutine_raise, 'coro')
    with self.assertRaises(BeaconException):
      self.r.step()
    self.assertTrue(self.r.done)

  def test_exception_recurse(self):
    self.r = sched.Coroutine(lambda: self.coroutine_raise_meta(), 'coro')
    self.r.step()
    with self.assertRaises(BeaconException):
      self.r.step()
    self.assertTrue(self.r.done)

class TestScheduler(unittest.TestCase):

  def __init__(self, *args, **kwargs):
    unittest.TestCase.__init__(self, *args, **kwargs)

  def setUp(self):
    self.scheduler = sched.Scheduler()
    self.beacon1 = 0
    self.beacon2 = 0

  def coroutine1(self):
    self.beacon1 += 1
    sched.coro_yield()
    self.beacon1 += 1

  def coroutine2(self):
    self.beacon2 += 1
    sched.coro_yield()
    self.beacon2 += 1

  def coroutine_wait(self, w):
    #self.assertEqual(self.beacon1, 1)
    sched.Coroutine.current.wait(w)
    #self.assertEqual(self.beacon1, 2)

  def test_basic(self):
    sched.Coroutine(self.coroutine1, 'coro', self.scheduler)
    self.assertEqual(self.beacon1, 0)
    self.scheduler.run()
    self.assertEqual(self.beacon1, 2)

  def test_several(self):
    sched.Coroutine(self.coroutine1, 'coro1', self.scheduler)
    sched.Coroutine(self.coroutine2, 'coro2', self.scheduler)
    self.assertEqual(self.beacon1, 0)
    self.assertEqual(self.beacon2, 0)
    self.scheduler.run()
    self.assertEqual(self.beacon1, 2)
    self.assertEqual(self.beacon2, 2)

  def test_wait(self):
    c1 = sched.Coroutine(self.coroutine1, 'coro1', self.scheduler)
    cw = sched.Coroutine(lambda: self.coroutine_wait(c1), 'coro_wait',
                   self.scheduler)
    self.scheduler.run()

  def test_reactor(self):

    class Sleep(sched.ThreadedOperation):
      def __init__(self, duration):
        sched.ThreadedOperation.__init__(self)
        self.__duration = duration

      def run(self):
        import time
        time.sleep(self.__duration)

    s = Sleep(1)
    cw = sched.Coroutine(lambda: self.coroutine_wait(s), 'coro_wait',
                   self.scheduler)
    sleeper = sched.Coroutine(s.start, 'coro_sleep', self.scheduler)
    self.scheduler.run()

  def test_fwd_exception(self):

    def coro():
      def subcoro():
        def raiser():
          raise BeaconException()
        r = sched.Coroutine(raiser, 'raiser', self.scheduler, sched.Coroutine.current)
        sched.coro_wait(r)
      sc = sched.Coroutine(subcoro, 'subcoro', self.scheduler, sched.Coroutine.current)
      with self.assertRaises(BeaconException):
        sched.coro_wait(sc)

    c = sched.Coroutine(coro, 'coro', self.scheduler)
    self.scheduler.run()

  def test_semaphore(self):

    s = sched.Semaphore(1)
    beacon = [0]
    def lock_f(beacon):
      for i in range(3):
        s.lock()
        beacon[0] = beacon[0] + 1
    lock = sched.Coroutine(lambda: lock_f(beacon), 'lock', self.scheduler)
    def read_f(beacon):
      def check(i):
        # Yield twice, to make sure lock_f has an execution slot: we
        # just woke him, so it might be scheduled after us in the next
        # round.
        sched.coro_yield()
        sched.coro_yield()
        assert beacon[0] == i
        sched.coro_yield()
        assert beacon[0] == i
        s.unlock()
        assert beacon[0] == i
      check(1)
      check(2)
      check(3)
    read = sched.Coroutine(lambda: read_f(beacon), 'read', self.scheduler)
    self.scheduler.run()

unittest.main()
