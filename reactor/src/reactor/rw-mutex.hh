#ifndef INFINIT_REACTOR_RW_MUTEX_HH
# define INFINIT_REACTOR_RW_MUTEX_HH

# include <reactor/mutex.hh>

# include <elle/attribute.hh>

namespace reactor
{
  class RWMutex
    : public Lockable
  {
  public:
    RWMutex();
    bool
    locked() const;
    virtual
    bool
    release() override;
    virtual
    bool
    acquire() override;

    class WriteMutex
      : public Lockable
    {
    public:
      WriteMutex(RWMutex& owner);
      bool
      locked() const;
      virtual
      bool
      release() override;
      virtual
      bool
      acquire() override;

    protected:
      virtual
      bool
      _wait(Thread* thread, Waker const& waker) override;
      virtual
      Thread*
      _signal_one() override;

    private:
      ELLE_ATTRIBUTE(RWMutex&, owner);
      ELLE_ATTRIBUTE(reactor::Thread*, locked);
      ELLE_ATTRIBUTE(int, locked_recursive);
      friend class RWMutex;
    };
    WriteMutex& write();

  protected:
    virtual
    bool
    _wait(Thread* thread, Waker const& waker) override;
  private:
    WriteMutex _write;
    ELLE_ATTRIBUTE_R(int, readers);
  };
}

#endif
