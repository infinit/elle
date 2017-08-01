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
      using Super = Logger;
      using Loggers = std::vector<std::unique_ptr<Logger>>;
      CompositeLogger(Loggers l);
      CompositeLogger();
      ELLE_ATTRIBUTE_RX(Loggers, loggers);
    protected:
      void
      _message(Message const& msg) override;

      // FIXME: we should only redefine _message.  But this move
      // relies on migrating to std::chrono and Hinnant's date library
      // so that we store the time in the Message and format it later.
      // This effort is already done in another branch, instead of
      // duplicating it now, just workaround.
      void message(Level level,
                   Type type,
                   std::string const& component,
                   std::string const& message,
                   std::string const& file,
                   unsigned int line,
                   std::string const& function) override;


      /// Whether is active for any of the subloggers.
      bool
      _component_is_active(std::string const& name, Level level) override;
    };
  }
}
