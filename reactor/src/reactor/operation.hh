#ifndef REACTOR_OPERATION_HH
# define REACTOR_OPERATION_HH

# include <reactor/duration.hh>
# include <reactor/waitable.hh>

namespace reactor
{
  class Operation: public Waitable
  {
  public:
    using Self = reactor::Operation;
    using Super = reactor::Waitable;
    Operation();
    Operation(Scheduler& scheduler);
    Scheduler& scheduler();
    bool run(DurationOpt timeout = DurationOpt());
    void start();

    ELLE_ATTRIBUTE_R(bool, running);

  protected:
    void abort();
    void done();
    virtual void _abort() = 0;
    virtual void _start() = 0;

  private:
    Scheduler& _sched;
  };
}

#endif
