#ifndef ELLE_CRASHREPORTER_HH
# define ELLE_CRASHREPORTER_HH

# include <elle/reactor/src/reactor/backtrace.hh>

# include <cstring>

# include <boost/system/error_code.hpp>
# include <boost/asio/signal_set.hpp>
# include <boost/signals.hpp>

namespace elle
{
  namespace signal
  {

    class ScoppedGuard:
      private boost::noncopyable
    {
      /*---------.
      | Typedefs |
      `---------*/
    private:
      /// Handler prototype.
      typedef
      boost::function<void(boost::system::error_code const&, int)> Handler;

      /// Attributes.
    private:
      boost::asio::signal_set _signals;
      Handler _handler;

      /*-------------.
      | Construction |
      `-------------*/
    public:
      /// Initialize signal list with scheduler io_service.
      ScoppedGuard();
      /// Initialize with signal list and handler.
      ScoppedGuard(std::vector<int> const& sig,
                   Handler const& handler);
      /// Destroy current handlers.
      ~ScoppedGuard();

      /*---------------.
      | Initialization |
      `---------------*/
    public:
      /// Register signals with handler.
      void
      init(std::vector<int> const& sig,
           Handler const& handler);

      /// Launch thread;
      void
      launch();

      /*------------.
      | Destruction |
      `------------*/
    public:
      /// Clear handler list.
      void
      release();
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
      operator() (boost::system::error_code const& error,
                  int sig);

    private:
      std::string _name;
      bool _quit;
    };

    bool
    report(std::string const& module,
           std::string const& signal = "",
           reactor::Backtrace const& b = reactor::Backtrace::current());
  }

}

#endif
