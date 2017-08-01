#pragma once

#include <elle/filesystem/path.hh>
#include <elle/fstream.hh>
#include <elle/log/TextLogger.hh>

namespace elle
{
  namespace log
  {
    /// A TextLogger that writes to named files.
    class ELLE_API FileLogger
      : public Logger
    {
    public:
      using Super = Logger;

      /// Create a log on file.
      ///
      /// @param base  the basename for the logs: base.0, base.1, etc.
      /// @param log_level  the log levels
      /// @param size   the max size of the log file before rotation
      ///             (same name as logrotate)
      /// @param rotate the max number of logs to keep.
      ///               0 to never remove.
      ///               (same name as logrotate)
      /// @param append whether to append to the last log.
      /// @param envvar  the name of the environment variable that
      ///            overrides the specified log level.
      FileLogger(fs::path base,
                 std::string const& log_level = "LOG",
                 size_t size = 0,
                 size_t rotate = 0,
                 bool append = false,
                 std::string const& envvar = "ELLE_LOG_LEVEL");

      /// Change the family name.  Renames the current file.
      void base(fs::path base);

    protected:
      void
      _message(Message const& msg) override;

      void
      _log_level(std::string const& log_level) override;

    private:
      ELLE_ATTRIBUTE_R(fs::path, base);
      ELLE_ATTRIBUTE_R(elle::ofstream, fstream);
      ELLE_ATTRIBUTE_RW(size_t, size);
      ELLE_ATTRIBUTE_RW(size_t, rotate);
      ELLE_ATTRIBUTE_RW(bool, append);
      // A pointer because we have to support G++ 4.9 which does
      // not support move for streams.
      // FIXME: C++17.
      ELLE_ATTRIBUTE_R(std::unique_ptr<TextLogger>, logger);
    };
  }
}
