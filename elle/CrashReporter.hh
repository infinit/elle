#ifndef ELLE_CRASHREPORTER_HH
# define ELLE_CRASHREPORTER_HH

# include <cstring>

# include <boost/system/error_code.hpp>
# include <boost/asio/signal_set.hpp>
# include <boost/signals.hpp>

namespace elle
{
  namespace crash
  {
    void
    report_handler(boost::system::error_code const& error,
                   int sig);
    void
    exiting_report_handler(boost::system::error_code const& error,
                           int sig);
    bool
    report(std::string const& module,
           std::string const& signal = "");
  }

  namespace signal
  {
    class ScoppedGuard:
      private boost::noncopyable
    {
      /// Types.
    private:
      typedef
      boost::function<void(boost::system::error_code const&, int)> Handler;

     /// Attributes.
    private:
      boost::asio::signal_set _signals;

    public:
      ScoppedGuard();

      ScoppedGuard(std::vector<int> const& sig,
                   Handler const& handler);

      void
      init(std::vector<int> const& sig,
           Handler const& handler);

      void
      release();
    };
  }
}

#endif
