#pragma once

# include <elle/Error.hh>
# include <elle/Exception.hh>
# include <elle/reactor/duration.hh>
# include <elle/reactor/fwd.hh>

namespace elle
{
  namespace reactor
  {
    /// Base type of Exception for the reactor.
    class Exception
      : public elle::Exception
    {
    public:
      /// Construct an Exception with a message to be displayed.
      ///
      /// \param message A message with more details about the exception.
      Exception(const std::string& message);
      /// Construct an Exception with a Backtrace.
      ///
      /// \param bt The Backtrace at the point the Exception was raised.
      /// \param message a message with more details about the exception.
      Exception(elle::Backtrace const& bt, std::string const& message);
    };

    /// Exception representing an action timing out.
    class Timeout
      : public elle::Error
    {
    public:
      using Super = elle::Error;
      /// Construct a Timeout exception with the Duration given to the action
      /// before it timed out.
      ///
      /// \param delay The Duration that was given to the action before it timed
      ///              out.
      Timeout(reactor::Duration const& delay);
      ELLE_ATTRIBUTE_R(reactor::Duration, delay);
    };

    /// Termination is a special type of Exception that allows for stopping
    /// a Thread execution.
    ///
    /// This exception is used by Scheduler::terminate and
    /// Scheduler::terminate_now.
    class Terminate
      : public elle::Exception
    {
    public:
      using Super = elle::Exception;
      /// Construct an Terminate exception.
      ///
      /// \param message A message with more details about the exception.
      Terminate(std::string const& message);
    };

    namespace filesystem
    {
      /// Exception type for filesystem errors. Filesystems must not throw
      /// anything else.
      class Error
        : public elle::Error
      {
      public:
        Error(int error_code, std::string const& message)
          : elle::Error(message)
          , _error_code(error_code)
        {}

        Error(int error_code, std::string const& message,
              elle::Backtrace const& bt)
          : elle::Error(bt, message)
          , _error_code(error_code)
        {}
        ELLE_ATTRIBUTE_R(int, error_code);
      };
    }
  }
}
