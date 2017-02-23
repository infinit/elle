#pragma once

# include <elle/reactor/asio.hh>
# include <elle/reactor/operation.hh>

namespace elle
{
  namespace reactor
  {
    class Sleep: public Operation
    {
    public:
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
