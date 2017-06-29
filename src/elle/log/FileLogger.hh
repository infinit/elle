#pragma once

#include <fstream>

#include <elle/log/TextLogger.hh>

namespace elle
{
  namespace log
  {
    /// A TextLogger that writes to rotating files.
    class ELLE_API FileLogger
      : public Logger
    {
    public:
      using Super = Logger;

      /// Create a log on file.
      ///
      /// @param base  the basename for the logs: base.0, base.1, etc.
      /// @param log_level  the log levels
      FileLogger(std::string const& base,
                 std::string const& log_level = "");

    protected:
      void
      _message(Level level,
               Type type,
               std::string const& component,
               Time const& time,
               std::string const& message,
               Tags const& tags,
               int indentation,
               std::string const& file,
               unsigned int line,
               std::string const& function) override;

      void
      _log_level(std::string const& log_level) override;

    private:
      ELLE_ATTRIBUTE_R(std::string const&, base);
      ELLE_ATTRIBUTE_R(std::ofstream, fstream);
      ELLE_ATTRIBUTE_RW(size_t, threshold);
      ELLE_ATTRIBUTE_R(TextLogger, logger);
    };
  }
}
