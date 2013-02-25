#ifndef INFINIT_REACTOR_MUTEX_HH
# define INFINIT_REACTOR_MUTEX_HH

# include <reactor/lockable.hh>

namespace reactor
{
  class Mutex: public Lockable
  {
    public:
      Mutex();
      bool locked() const;
      virtual bool release();

    protected:
      virtual bool _wait(Thread* thread);

    private:
      bool _locked;
  };
}

#endif
