#include <elle/bytes.hh>
#include <elle/fstream.hh>
#include <elle/log/FileLogger.hh>

namespace elle
{
  namespace log
  {
    FileLogger::FileLogger(fs::path base_path,
                           std::string const& log_level,
                           size_t size,
                           size_t rotate,
                           bool append,
                           std::string const& envvar)
      : Super{log_level, envvar}
      , _base{std::move(base_path)}
      , _size{size}
      , _rotate{rotate}
      , _append{append}
    {
      struct base;
      if (this->size())
        elle::rotate(this->_fstream, this->_base, this->size(), this->rotate(),
                     this->append());
      else if (this->append())
        this->_fstream.open(this->base(),
                            std::fstream::app | std::fstream::out);
      else
        this->_fstream.open(this->base(),
                            std::fstream::trunc | std::fstream::out);
      this->_logger
        = std::make_unique<TextLogger>(this->_fstream, log_level, envvar);
    }

    void
    FileLogger::base(fs::path base)
    {
      auto path
        = fs::path(boost::replace_head_copy(this->_fstream.path().string(),
                                            this->_base.string().size(),
                                            base.string()));
      this->_base = std::move(base);
      this->_fstream.path(std::move(path));
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
        elle::rotate(this->_fstream, this->size(), this->rotate());
    }
  }
}
