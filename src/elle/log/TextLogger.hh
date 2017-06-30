#pragma once

#include <iosfwd>

#include <elle/log/Logger.hh>

namespace elle
{
  namespace log
  {
    class ELLE_API TextLogger
      : public Logger
    {
    public:
      TextLogger(std::ostream& out,
                 std::string const& log_level = "",
                 bool display_type = false,
                 bool enable_pid = false,
                 bool enable_tid = false,
                 bool enable_time = false,
                 bool universal_time = false,
                 bool microsec_time = false,
                 bool warn_err_only = false);
    protected:
      void
      _message(Message const& msg) override;

    private:
      ELLE_ATTRIBUTE_R(std::ostream&, output);
      ELLE_ATTRIBUTE_RW(bool, display_type);
      ELLE_ATTRIBUTE_RW(bool, enable_pid);
      ELLE_ATTRIBUTE_RW(bool, enable_tid);
      ELLE_ATTRIBUTE_RW(bool, enable_time);
      ELLE_ATTRIBUTE_RW(bool, warn_err_only);
    };
  }
}
