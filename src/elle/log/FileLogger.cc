#include <elle/bytes.hh>
#include <elle/fstream.hh>
#include <elle/log/FileLogger.hh>

namespace elle
{
  namespace log
  {
    FileLogger::FileLogger(std::string const& base,
                           std::string const& log_level,
                           size_t threshold,
                           bool append)
      : Super{log_level}
      , _base(base)
      , _threshold{threshold}
      , _append{append}
    {
      if (this->_threshold)
        rotate(this->_fstream, this->base(), this->threshold());
      else if (append)
        this->_fstream.open(this->base(),
                            std::fstream::app | std::fstream::out);
      else
        this->_fstream.open(this->base(),
                            std::fstream::trunc | std::fstream::out);
      this->_logger
        = std::make_unique<TextLogger>(this->_fstream, log_level);
    }

    void
    FileLogger::_log_level(std::string const& log_level)
    {
      this->_logger->log_level(log_level);
    }

    void
    FileLogger::_message(Message const& msg)
    {
      this->_logger->message(msg);
      if (this->threshold())
        rotate(this->_fstream, this->base(), this->threshold());
    }
  }
}
