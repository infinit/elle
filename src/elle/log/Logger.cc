#include <fnmatch.h>

#include <boost/algorithm/string.hpp>

#include <elle/log/Logger.hh>

namespace elle
{
  namespace log
  {
    void
    Logger::message(Level level,
                    elle::log::Logger::Type type,
                    std::string const& msg)
    {
      this->_message(level, type, msg);
    }

#define ELLE_LOG_LEVEL_MESSAGE(Lvl)                     \
    void                                                \
    Logger::Lvl(std::string const& msg)                 \
    {                                                   \
    return this->message(Logger::Level::Lvl,            \
                         Logger::Type::info,            \
                         msg);                          \
    }                                                   \

    ELLE_LOG_LEVEL_MESSAGE(log);
    ELLE_LOG_LEVEL_MESSAGE(trace);
    ELLE_LOG_LEVEL_MESSAGE(debug);
    ELLE_LOG_LEVEL_MESSAGE(dump);
#undef ELLE_LOG_LEVEL_MESSAGE

    Logger::Logger()
      : _component_patterns()
      , _component_enabled()
      , _component_max_size(0)
    {
      static char const* components_str = ::getenv("ELLE_LOG_COMPONENTS");
      if (components_str == nullptr)
        this->_component_patterns.push_back("*");
      else
        {
          boost::algorithm::split(this->_component_patterns, components_str,
                                  boost::algorithm::is_any_of(","),
                                  boost::algorithm::token_compress_on);
          for (auto& pattern: this->_component_patterns)
            boost::algorithm::trim(pattern);
        }
    }

    Logger::~Logger()
    {}

    bool
    Logger::component_enabled(std::string const& name)
    {
      auto elt = this->_component_enabled.find(name);
      if (elt == this->_component_enabled.end())
        {
          bool res = false;
          for (auto const& pattern: this->_component_patterns)
            {
              if (fnmatch(pattern.c_str(), name.c_str(), 0) == 0)
                {
                  this->_component_max_size =
                    std::max(this->_component_max_size,
                             static_cast<unsigned int>(name.size()));
                  res = true;
                  break;
                }
            }
          this->_component_enabled[name] = res;
          return res;
        }
      else
        return elt->second;
    }
  }
}
