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
      ELLE_ATTRIBUTE_RX(std::vector<std::unique_ptr<Logger>>, loggers);
    protected:
      void
      _message(Level level,
               elle::log::Logger::Type type,
               std::string const& component,
               Time const& time,
               std::string const& message,
               Tags const& tags,
               int indentation,
               std::string const& file,
               unsigned int line,
               std::string const& function) override;
    };
  }
}
