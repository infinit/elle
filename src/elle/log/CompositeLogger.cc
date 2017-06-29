#include <elle/log/CompositeLogger.hh>

namespace elle
{
  namespace log
  {
    CompositeLogger::CompositeLogger()
      : Logger("LOG")
    {}

    void
    CompositeLogger::_message(Level level,
                              elle::log::Logger::Type type,
                              std::string const& component,
                              Time const& time,
                              std::string const& message,
                              Tags const& tags,
                              int indentation,
                              std::string const& file,
                              unsigned int line,
                              std::string const& function)
    {
      // We bounce to message and not _message so that each child logger can
      // have its own settings.
      // We must reproduce Send's behavior regarding indent and categories.
      for (auto& l: this->_loggers)
      {
        l->indentation() = indentation + 1;
        l->component_level(component); // for max size computation
        l->message(level, type, component, message, file, line, function);
      }
    }
  }
}
