#ifndef REACTOR_OPERATION_HH
# define REACTOR_OPERATION_HH

# include <reactor/waitable.hh>

namespace reactor
{
  class Operation: public Waitable
  {
    public:
      typedef Operation Self;
      typedef Waitable Super;
      Operation(Scheduler& scheduler);
      void abort();
      void start();
      Scheduler& scheduler();
      bool run(DurationOpt timeout = DurationOpt());

    protected:
      virtual void _abort() = 0;
      virtual void _start() = 0;

    private:
      Scheduler& _sched;
  };
}

#endif
