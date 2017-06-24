#include <elle/log/TextLogger.hh>

#include <iostream>
#include <unistd.h>

#include <boost/algorithm/string/classification.hpp>
#include <boost/algorithm/string/split.hpp>
#include <boost/algorithm/string/trim.hpp>
#include <boost/date_time/posix_time/posix_time.hpp>

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

    namespace
    {
      std::string
      _type_to_string(Logger::Type type)
      {
        switch (type)
        {
        case Logger::Type::info:
          return "info";
        case Logger::Type::warning:
          return "warning";
        case Logger::Type::error:
          return "error";
        }
        elle::unreachable();
      }
    }

    TextLogger::TextLogger(std::ostream& out,
                           std::string const& log_level,
                           bool display_type,
                           bool enable_pid,
                           bool enable_tid,
                           bool enable_time,
                           bool universal_time,
                           bool microsec_time,
                           bool warn_err_only)
      : Logger(log_level)
      , _output(out)
      , _display_type(os::getenv("ELLE_LOG_DISPLAY_TYPE", display_type))
      , _enable_pid(os::getenv("ELLE_LOG_PID", enable_pid))
      , _enable_tid(os::getenv("ELLE_LOG_TID", enable_tid))
      , _enable_time(os::getenv("ELLE_LOG_TIME", enable_time))
      , _warn_err_only(warn_err_only)
    {
      this->time_universal(os::getenv("ELLE_LOG_TIME_UNIVERSAL",
                                            universal_time));
      this->time_microsec(os::getenv("ELLE_LOG_TIME_MICROSEC",
                                           microsec_time));
    }

    namespace
    {
      std::string
      get_color_code(Logger::Level level, Logger::Type type)
      {
        static bool c = os::getenv("ELLE_LOG_COLOR", true);
        if (c)
          switch (type)
          {
          case Logger::Type::info:
            if (level == Logger::Level::log)
              return "[1m";
            else
              return "";
          case Logger::Type::warning:
            // Yellow
            return "[33;01;33m";
          case Logger::Type::error:
            // Red
            return "[33;01;31m";
          }
        else
          return "";
        elle::unreachable();
      }
    }

    void
    TextLogger::_message(
      Level level,
      elle::log::Logger::Type type,
      std::string const& component,
      boost::posix_time::ptime const& time,
      std::string const& message,
      Tags const& tags,
      int indentation,
      std::string const& file,
      unsigned int line,
      std::string const& function)
    {
      if (_warn_err_only && type < Type::warning)
        return;
      auto lines = std::vector<std::string>{};
      {
        auto trimmed = boost::algorithm::trim_copy_if(
          message,
          boost::algorithm::is_any_of(" \t\n\r"));
        boost::split(lines,
                     trimmed,
                     boost::algorithm::is_any_of("\r\n"),
                     boost::token_compress_on);
        ELLE_ASSERT(!lines.empty());
      }
      auto msg = lines[0];

      // Indentation
      static bool const display_indentation
        = !os::getenv("ELLE_LOG_NO_INDENTATION", false);
      if (display_indentation)
      {
        auto align = std::string(indentation * 2, ' ');
        msg = align + msg;
      }

      // Location
      static bool const location = os::getenv("ELLE_LOG_LOCATIONS", false);
      if (location)
        msg = elle::sprintf("%s (at %s:%s in %s)", msg, file, line, function);

      // Type
      if (this->_display_type && type != elle::log::Logger::Type::info)
        msg = elle::sprintf("[%s] ", _type_to_string(type)) + msg;

      static bool display_tags = !os::getenv("ELLE_LOG_NO_TAGS", false);
      // Tags
      if (display_tags)
        for (auto const& tag: tags)
        {
          if (tag.first == "PID" && !this->_enable_pid)
            continue;
          if (tag.first == "TID" && !this->_enable_tid)
            continue;
          msg = elle::sprintf("[%s] %s", tag.second, msg);
        }

      static bool show_component = !os::getenv("ELLE_LOG_NO_COMPONENT", false);
      // Component
      std::string comp;
      if (show_component)
      {
        auto size = component.size();
        ELLE_ASSERT_LTE(size, this->component_max_size());
        auto pad = this->component_max_size() - size;
        comp = "[" + std::string(pad / 2, ' ')
          + component + std::string(pad / 2 + pad % 2, ' ')
          + "] ";
      }
      msg = comp + msg;

      // Time
      if (this->_enable_time)
        msg = elle::sprintf("%s: %s", time, msg);

      auto color_code = get_color_code(level, type);
      this->_output << color_code << msg << std::endl;

      if (lines.size() > 1)
      {
        ELLE_ASSERT_GTE(msg.size(), lines[0].size());
        auto indent = std::string(msg.size() - lines[0].size(), ' ');
        for (auto i = 1u; i < lines.size(); i++)
          this->_output << indent << lines[i] << std::endl;
      }
      if (!color_code.empty())
        this->_output << "[0m";
      this->_output.flush();
    }
  }
}
