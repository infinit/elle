#include <unistd.h>

#include <iostream>

#include <elle/Exception.hh>
#include <elle/log/TextLogger.hh>
#include <elle/printf.hh>

namespace elle
{
  namespace log
  {
    static
    bool
    color()
    {
      static const char* color_env_name = "ELLE_LOG_COLOR";
      char* color_env = ::getenv(color_env_name);
      if (color_env)
        {
          std::string c(color_env);
          if (c == "0")
            return false;
          else if (c == "1")
            return true;
          else
            throw elle::Exception(elle::sprintf("invalid value for %s: %s",
                                                color_env_name, c));
        }
      else
        return true;
    }

    TextLogger::TextLogger(std::ostream& out)
      : _output(out)
    {}


    void
    TextLogger::_message(Level level,
                         elle::log::Logger::Type type,
                         std::string const& component,
                         std::string const& message,
                         std::string const& file,
                         unsigned int line,
                         std::string const& function)
    {
      static bool c = color();
      std::string color_code;
      if (c)
        switch (type)
        {
          case Type::info:
            if (level == Level::log)
              color_code = "[1m";
            break;
          case Type::warning:
            // Yellow
            color_code = "[33;01;33m";
            break;
          case Type::error:
            // Red
            color_code = "[33;01;31m";
            break;
        }
      _output << color_code;
      _output << message << std::endl;
      if (!color_code.empty())
        _output << "[0m";
      _output.flush();
    }
  }
}
