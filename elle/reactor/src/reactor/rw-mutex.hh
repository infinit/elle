#ifndef INFINIT_REACTOR_RW_MUTEX_HH
# define INFINIT_REACTOR_RW_MUTEX_HH

# include <reactor/mutex.hh>

namespace reactor
{
  class RWMutex: public Lockable
  {
    public:
      RWMutex();
      virtual bool release();

      class WriteMutex: public Lockable
      {
        public:
          WriteMutex(RWMutex& owner);
          virtual bool release();
        protected:
          virtual bool _wait(Thread* thread);
        private:
          RWMutex& _owner;
          reactor::Thread* _locked;
          int _locked_recursive;
          friend class RWMutex;
      };
      WriteMutex& write();

    protected:
      virtual bool _wait(Thread* thread);
    private:
      WriteMutex _write;
      int _readers;
  };
}

#endif
