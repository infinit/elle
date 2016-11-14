#include <elle/log/CompositeLogger.hh>


namespace elle
{
  namespace log
  {
    CompositeLogger::CompositeLogger()
      : Logger("LOG")
    {}

    CompositeLogger::~CompositeLogger()
    {}

    void
    CompositeLogger::_message(Level level,
               elle::log::Logger::Type type,
               std::string const& component,
               boost::posix_time::ptime const& time,
               std::string const& message,
               std::vector<std::pair<std::string, std::string>> const& tags,
               int indentation,
               std::string const& file,
               unsigned int line,
               std::string const& function)
    {
      /* We bounce to message and not _message so that each child logger can
         have its own settings.
         We must reproduce Send's behavior regarding indent and categories.
      */
      for (auto& l: _loggers)
      {
        l->indentation() = indentation + 1;
        l->component_enabled(component); // for max size computation
        l->message(level, type, component, message, file, line, function);
      }
    }
  }
}
