#include <elle/bytes.hh>
#include <elle/fstream.hh>
#include <elle/log/FileLogger.hh>

namespace elle
{
  namespace log
  {
    FileLogger::FileLogger(std::string const& base,
                           std::string const& log_level)
      : Super{log_level}
      , _base(base)
      , _threshold{100_KiB}
    {
      rotate(this->_fstream, this->base(), this->threshold());
      this->_logger = std::make_unique<TextLogger>(this->_fstream, log_level);
    }

    void
    FileLogger::_log_level(std::string const& log_level)
    {
      this->_logger->log_level(log_level);
    }

    void
    FileLogger::_message(Level level,
                         Type type,
                         std::string const& component,
                         Time const& time,
                         std::string const& message,
                         Tags const& tags,
                         int indentation,
                         std::string const& file,
                         unsigned int line,
                         std::string const& function)
    {
      this->_logger->message(level, type, component
                            // , time
                            , message
                            // , tags
                            // , indentation
                            , file, line, function
                            );
      rotate(this->_fstream, this->base(), this->threshold());
    }
  }
}
