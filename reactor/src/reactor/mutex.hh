#ifndef INFINIT_REACTOR_MUTEX_HH
# define INFINIT_REACTOR_MUTEX_HH

# include <reactor/lockable.hh>

namespace reactor
{
  class Mutex: public Lockable
  {
  public:
    Mutex();
    virtual
    ~Mutex();
    Mutex(Mutex&&) = default;
    bool locked() const;
    virtual bool release() override;
    virtual bool acquire() override;
  protected:
    virtual bool _wait(Thread* thread, Waker const& waker) override;

  private:
    bool _locked;
  };
}

#endif
