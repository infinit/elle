#ifndef REACTOR_SCOPE_HH
# define REACTOR_SCOPE_HH

# include <reactor/fwd.hh>
# include <reactor/thread.hh>

namespace reactor
{
  class Scope
  {
  public:
    Scope();
    ~Scope();
    void
    run_background(std::string const& name,
                   Thread::Action const& a);

  private:
    std::vector<Thread*> _threads;
  };
}


#endif
