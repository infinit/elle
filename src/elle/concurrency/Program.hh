#ifndef ELLE_CONCURRENCY_PROGRAM_HH
# define ELLE_CONCURRENCY_PROGRAM_HH

# include <elle/types.hh>

# include <reactor/signal.hh>

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

      static
      Status
      Setup(std::string const& name, std::string const& host, int port);

      static
      void
      Launch();

      static
      void
      Exception(int signal);

      static
      void
      Exit();

      //
      // attributes
      //
    private:
      static std::string _name;
      static std::string _host;
      static reactor::Signal _exit;
      static int _port;
    };

  }
}

#endif
