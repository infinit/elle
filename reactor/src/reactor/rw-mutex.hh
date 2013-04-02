#ifndef INFINIT_REACTOR_RW_MUTEX_HH
# define INFINIT_REACTOR_RW_MUTEX_HH

# include <reactor/mutex.hh>

# include <elle/attribute.hh>

namespace reactor
{
  class RWMutex: public Lockable
  {
    public:
      RWMutex();
    bool
    locked() const;
      virtual bool release();

      class WriteMutex: public Lockable
      {
        public:
          WriteMutex(RWMutex& owner);
        bool
        locked() const;
          virtual bool release();
        protected:
          virtual bool _wait(Thread* thread);
          virtual Thread* _signal_one();
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
    ELLE_ATTRIBUTE_R(int, readers);
  };
}

#endif
