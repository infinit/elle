#ifndef ELLE_CRASHREPORTER_HH
# define ELLE_CRASHREPORTER_HH

# include <reactor/backtrace.hh>

# include <cstring>

# include <boost/system/error_code.hpp>
# include <boost/asio/signal_set.hpp>
# include <boost/signals.hpp>

namespace elle
{
  namespace signal
  {

    class ScopedGuard:
      private boost::noncopyable
    {
      /*---------.
      | Typedefs |
      `---------*/
    private:
      /// Handler prototype.
      typedef
      std::function<void(boost::system::error_code const&, int)> Handler;

      /// Attributes.
    private:
      boost::asio::signal_set _signals;
      Handler _handler;

      /*-------------.
      | Construction |
      `-------------*/
    public:
      /// Initialize with signal list and handler.
      ScopedGuard(reactor::Scheduler& sched,
                  std::vector<int> const& sig,
                  Handler const& handler);
      /// Destroy current handlers.
      ~ScopedGuard();

      /*---------------.
      | Initialization |
      `---------------*/
      /// Launch thread;
    private:
      void
      _launch();

      /*------------.
      | Destruction |
      `------------*/
    private:
      /// Clear handler list.
      void
      _release();
    };
  } // End of signal.

  namespace crash
  {

    class Handler
    {
    public:
      Handler(std::string const& name,
              bool quit);

      Handler(std::string const& name,
              bool quit,
              int argc,
              char** argv);

      virtual ~Handler();

      virtual
      void
      operator() (std::string const& host, int port,
                  boost::system::error_code const& error, int sig);

    private:
      std::string _name;
      bool _quit;
    };

    bool
    report(std::string const& host,
           int port,
           std::string const& module,
           std::string const& signal = "",
           reactor::Backtrace const& b = reactor::Backtrace::current());
  }

}

#endif
