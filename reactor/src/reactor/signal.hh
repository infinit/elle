#ifndef INFINIT_REACTOR_SIGNAL_HH
# define INFINIT_REACTOR_SIGNAL_HH

# include <reactor/waitable.hh>

namespace reactor
{
  class Signal: public Waitable
  {
    public:
      typedef Signal Self;
      typedef Waitable Super;
      Signal(const std::string& name = std::string());
      void signal();
  };
}

#endif
