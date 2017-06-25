#pragma once

#include <elle/reactor/asio.hh>
#include <elle/reactor/Operation.hh>

namespace elle
{
  namespace reactor
  {
    /// Sleep is an Operation that does nothing but waiting for a certain
    /// duration
    ///
    /// Sleep is, for example, used by Thread::sleep(duration).
    ///
    /// @code{.cc}
    ///
    /// std::time_t start = std::time(NULL);
    /// auto sleep = elle::reactor::Sleep{elle::reactor::Scheduler::scheduler(),
    ///                                   2_sec};
    /// sleep.run();
    /// std::cout << std::difftime(std::time(NULL), start) << " s";
    ///
    /// // Result: 2 s
    ///
    /// @endcode
    class Sleep
      : public Operation
    {
    public:
      /// Construct a Sleep.
      ///
      /// @param scheduler The Scheduler to run the Operation.
      /// @param d The Duration the Thread must sleep.
      Sleep(Scheduler& scheduler, Duration d);

    protected:
      void
      _abort() override;
      void
      _start() override;

    /*----------.
    | Printable |
    `----------*/
    public:
      void
      print(std::ostream& stream) const override;

    private:
      void
      _wakeup(const boost::system::error_code& error);
      Duration _duration;
      boost::asio::deadline_timer _timer;
    };
  }
}
