#include <elle/log/TextLogger.hh>

#include <iostream>
#include <unistd.h>

#include <boost/algorithm/string/classification.hpp>
#include <boost/algorithm/string/split.hpp>
#include <boost/algorithm/string/trim.hpp>
#include <boost/date_time/posix_time/posix_time.hpp>
#include <boost/lexical_cast.hpp>

#include <elle/Exception.hh>
#include <elle/assert.hh>
#include <elle/os/environ.hh>
#include <elle/printf.hh>

namespace elle
{
  namespace log
  {
    /*-----.
    | Type |
    `-----*/

    static
    std::string
    _type_to_string(elle::log::Logger::Type type)
    {
      switch (type)
      {
        case elle::log::Logger::Type::info:
          return "info";
        case elle::log::Logger::Type::warning:
          return "warning";
        case elle::log::Logger::Type::error:
          return "error";
        default:
          throw elle::Exception(elle::sprintf("unknown log type: %s", type));
      }
    }

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

    TextLogger::TextLogger(std::ostream& out,
                           std::string const& log_level,
                           bool display_type,
                           bool enable_pid,
                           bool enable_tid,
                           bool enable_time,
                           bool universal_time,
                           bool microsec_time,
                           bool warn_err_only):
      Logger(log_level),
      _output(out),
      _warn_err_only(warn_err_only)
    {
      if (elle::os::inenv("ELLE_LOG_DISPLAY_TYPE"))
      {
        this->_display_type =
          boost::lexical_cast<bool>(elle::os::getenv("ELLE_LOG_DISPLAY_TYPE"));
      }
      else
      {
        this->_display_type = display_type;
      }
      if (elle::os::inenv("ELLE_LOG_PID"))
      {
        this->_enable_pid =
          boost::lexical_cast<bool>(elle::os::getenv("ELLE_LOG_PID"));
      }
      else
      {
        this->_enable_pid = enable_pid;
      }
      if (elle::os::inenv("ELLE_LOG_TID"))
      {
        this->_enable_tid =
          boost::lexical_cast<bool>(elle::os::getenv("ELLE_LOG_TID"));
      }
      else
      {
        this->_enable_tid = enable_tid;
      }
      if (elle::os::inenv("ELLE_LOG_TIME"))
      {
        this->_enable_time =
          boost::lexical_cast<bool>(elle::os::getenv("ELLE_LOG_TIME"));
      }
      else
      {
        this->_enable_time = enable_time;
      }
      if (elle::os::inenv("ELLE_LOG_TIME_UNIVERSAL"))
      {
        this->time_universal(boost::lexical_cast<bool>(
          elle::os::getenv("ELLE_LOG_TIME_UNIVERSAL")));
      }
      else
      {
        this->time_universal(universal_time);
      }
      if (elle::os::inenv("ELLE_LOG_TIME_MICROSEC"))
      {
        this->time_microsec(boost::lexical_cast<bool>(
          elle::os::getenv("ELLE_LOG_TIME_MICROSEC")));
      }
      else
      {
        this->time_microsec(microsec_time);
      }
    }


    void
    TextLogger::_message(
      Level level,
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
      if (_warn_err_only && type < Type::warning)
        return;
      std::string trimmed_message = message;
      boost::algorithm::trim_if(
        trimmed_message,
        boost::algorithm::is_any_of(" \t\n\r"));
      std::vector<std::string> lines;
      boost::split(lines,
                   trimmed_message,
                   boost::algorithm::is_any_of("\r\n"),
                   boost::token_compress_on);
      ELLE_ASSERT(lines.size() > 0);
      std::string msg = lines[0];

      // Indentation
      static bool const display_indentation = !::getenv("ELLE_LOG_NO_INDENTATION");
      if (display_indentation)
      {
        std::string align = std::string(indentation * 2, ' ');
        msg = align + msg;
      }

      // Location
      static bool const location = ::getenv("ELLE_LOG_LOCATIONS");
      if (location)
        msg = elle::sprintf("%s (at %s:%s in %s)", msg, file, line, function);

      // Type
      if (this->_display_type && type != elle::log::Logger::Type::info)
        msg = elle::sprintf("[%s] ", _type_to_string(type)) + msg;

      static bool display_tags = !::getenv("ELLE_LOG_NO_TAGS");
      // Tags
      if (display_tags)
      {
        for (auto const& tag: tags)
        {
          if (tag.first == "PID" && !this->_enable_pid)
            continue;
          if (tag.first == "TID" && !this->_enable_tid)
            continue;
          msg = elle::sprintf("[%s] %s", tag.second, msg);
        }
      }

      static bool show_component = !::getenv("ELLE_LOG_NO_COMPONENT");
      // Component
      std::string comp;
      if (show_component)
      {
        unsigned int size = component.size();
        ELLE_ASSERT_LTE(size, this->component_max_size());
        unsigned int pad = this->component_max_size() - size;
        comp = std::string("[") + std::string(pad / 2, ' ')
          + component + std::string(pad / 2 + pad % 2, ' ')
          + std::string("] ");
      }
      msg = comp + msg;

      // Time
      if (this->_enable_time)
        msg = elle::sprintf("%s: %s", time, msg);

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
      _output << msg << std::endl;
      if (lines.size() > 1)
      {
          ELLE_ASSERT_GTE(msg.size(), lines[0].size());
          std::string indent(msg.size() - lines[0].size(), ' ');
          for (elle::Size i = 1; i < lines.size(); i++)
              _output << indent << lines[i] << std::endl;
      }
      if (!color_code.empty())
        _output << "[0m";
      _output.flush();
    }
  }
}
