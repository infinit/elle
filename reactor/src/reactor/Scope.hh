#ifndef REACTOR_SCOPE_HH
# define REACTOR_SCOPE_HH

# include <reactor/fwd.hh>

namespace reactor
{
  class Scope
  {
  public:
    Scope();
    ~Scope();
    void
    run_background(std::string const& name,
                   reactor::Thread::Action const& a);

  private:
    std::vector<Thread*> _threads;
  };
}


#endif
