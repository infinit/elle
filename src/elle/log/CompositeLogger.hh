#pragma once

#include <elle/log/Logger.hh>

namespace elle
{
  namespace log
  {
    /// A Logger that aggregates several others.
    class ELLE_API CompositeLogger
      : public Logger
    {
    public:
      CompositeLogger();
      ELLE_ATTRIBUTE_RX(std::vector<std::unique_ptr<Logger>>, loggers);
    protected:
      void
      _message(Message const& msg) override;

      /// Whether is active for any of the subloggers.
      bool
      _component_is_active(std::string const& name, Level level) override;
    };
  }
}
