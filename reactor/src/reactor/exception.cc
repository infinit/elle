#include <iostream>
#include <reactor/exception.hh>
#include <reactor/scheduler.hh>
#include <reactor/thread.hh>

#include <elle/printf.hh>

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
