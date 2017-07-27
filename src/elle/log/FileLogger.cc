#include <elle/bytes.hh>
#include <elle/fstream.hh>
#include <elle/log/FileLogger.hh>

namespace elle
{
  namespace log
  {
    FileLogger::FileLogger(fs::path base,
                           std::string const& log_level,
                           size_t size,
                           size_t rotate,
                           bool append)
      : Super{log_level}
      , _base{std::move(base)}
      , _size{size}
      , _rotate{rotate}
      , _append{append}
    {
      if (this->size())
        elle::rotate(this->_fstream, this->base(), this->size(), this->rotate(),
                     this->append());
      else if (this->append())
        this->_fstream.open(this->base().string(),
                            std::fstream::app | std::fstream::out);
      else
        this->_fstream.open(this->base().string(),
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
      if (this->size())
        elle::rotate(this->_fstream, this->base(), this->size(), this->rotate());
    }
  }
}
