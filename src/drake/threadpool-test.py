#!/usr/bin/env python3

import sys
import threadpool
import threading
import unittest

class TestStandaloneCoroutine(unittest.TestCase):

  def setUp(self):
    self.__pool = threadpool.ThreadPool()
    self.__beacon = 0
    self.__lock = threading.Semaphore(0)

  def tearDown(self):
    self.__pool.stop()

  def inc(self):
    self.__beacon += 1
    self.__lock.release()

  def test_empty(self):
    pass

  def test_one(self):
    self.__pool.run(self.inc)
    self.__lock.acquire()
    assert self.__beacon == 1

unittest.main()
