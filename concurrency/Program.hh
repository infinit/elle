#ifndef ELLE_CONCURRENCY_PROGRAM_HH
# define ELLE_CONCURRENCY_PROGRAM_HH

# include <elle/types.hh>

# include <elle/idiom/Close.hh>
#  include <reactor/signal.hh>
# include <elle/idiom/Open.hh>

// XXX
namespace reactor
{
  class Scheduler;
}

namespace elle
{
  namespace concurrency
  {

    ///
    /// this class represents the running program.
    ///
    class Program
    {
    public:
      //
      // static methods
      //
      static Status             Setup(std::string const& name);
      static void               Launch();
      static Void               Exception(int);
      static void               Exit();

      //
      // attributes
      //
    private:
      static std::string        _name;
      static reactor::Signal    _exit;
    };

  }
}

#endif
