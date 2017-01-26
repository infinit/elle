#pragma once

#include <elle/log/Logger.hh>

namespace elle
{
  namespace log
  {
    class ELLE_API CompositeLogger
      : public Logger
    {
    public:
      CompositeLogger();
      virtual
      ~CompositeLogger();
      ELLE_ATTRIBUTE_RX(std::vector<std::unique_ptr<Logger>>, loggers);
    protected:
      void
      _message(Level level,
               elle::log::Logger::Type type,
               std::string const& component,
               boost::posix_time::ptime const& time,
               std::string const& message,
               std::vector<std::pair<std::string, std::string>> const& tags,
               int indentation,
               std::string const& file,
               unsigned int line,
               std::string const& function) override;
      int _last_indent;
    };
  }
}
