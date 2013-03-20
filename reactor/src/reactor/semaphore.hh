#ifndef INFINIT_REACTOR_SEMAPHORE_HH
# define INFINIT_REACTOR_SEMAPHORE_HH

# include <reactor/waitable.hh>

namespace reactor
{
  class Semaphore: public Waitable
  {
    public:
      Semaphore(int count = 0);
      int count() const;
      bool release();

    protected:
      virtual bool _wait(Thread* thread);

    private:
      int _count;
  };
}

#endif
