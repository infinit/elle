import sys
import threading

class ThreadPool:

  class Runner(threading.Thread):

    def __init__(self, pool):
      threading.Thread.__init__(self)
      self.__cond = threading.Condition()
      self.__f = None
      self.__pool = pool
      self.__ready = False
      self.__stop = False
      self.start()

    def run(self):
      while True:
        #print('%s: process' % self, file = sys.stderr)
        with self.__cond:
          if self.__stop:
            #print('%s: stop' % self)
            self.__pool._ThreadPool__threads.remove(self)
            return
          if self.__f is None:
            #print('%s: wait for job' % self, file = sys.stderr)
            self.__cond.wait()
          if self.__stop:
            #print('%s: stop' % self, file = sys.stderr)
            self.__pool._ThreadPool__threads.remove(self)
            return
        #print('%s: run job' % self, file = sys.stderr)
        self.__f()
        self.__f = None
        with self.__pool._ThreadPool__lock:
          #print('%s: register as available' % self, file = sys.stderr)
          self.__pool._ThreadPool__running.remove(self)
          self.__pool._ThreadPool__threads.add(self)

    def wake(self, f):
      with self.__cond:
        self.__f = f
        self.__cond.notify()

    def stop(self):
      with self.__cond:
        self.__stop = True
        self.__cond.notify()
      self.join()

  def __init__(self):
    self.__threads = set()
    self.__running = set()
    self.__lock = threading.Semaphore(1)
    self.__cond = threading.Condition()

  def run(self, f):
    with self.__lock:
      if self.__threads:
        thread = iter(self.__threads).__next__()
        self.__threads.remove(thread)
      else:
        thread = ThreadPool.Runner(self)
      self.__running.add(thread)
    thread.wake(f)

  def stop(self):
    threads = set()
    # Collect the Threads and then stop them. Do not stop the Threads
    # while holding self.__lock, because Thread.stop joins the thread,
    # which might require self.__lock to finish, hence a dead-lock.
    with self.__lock:
      threads = threads.union(self.__threads)
      threads = threads.union(self.__running)
    for thread in threads:
      thread.stop()
