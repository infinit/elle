#include <iostream>

#include <elle/printf.hh>
#include <elle/reactor/exception.hh>
#include <elle/reactor/scheduler.hh>
#include <elle/reactor/Thread.hh>

namespace elle
{
  namespace reactor
  {
    Exception::Exception(const std::string& message)
      : Exception(elle::Backtrace(), message)
    {}

    Exception::Exception(elle::Backtrace const& bt, const std::string& message)
      : elle::Exception(bt, message)
    {}

    Timeout::Timeout(reactor::Duration const& delay)
      : Super(elle::sprintf("timeout %s", delay))
      , _delay(delay)
    {}

    Terminate::Terminate(const std::string& message)
      : Super(elle::sprintf("thread termination: %s", message))
    {}
  }
}
