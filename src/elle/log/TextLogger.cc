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
#include <elle/print.hh>

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
    TextLogger::_message(Message const& msg)
    {
      if (_warn_err_only && msg.type < Type::warning)
        return;
      // Split the message into lines.
      auto const lines = [&msg]
      {
        auto res = std::vector<std::string>{};
        auto trimmed = boost::algorithm::trim_copy_if(
          msg.message,
          boost::algorithm::is_any_of(" \t\n\r"));
        boost::split(res,
                     trimmed,
                     boost::algorithm::is_any_of("\r\n"),
                     boost::token_compress_on);
        ELLE_ASSERT(!res.empty());
        return res;
      }();

      // The first line has a complete header.
      auto const first_line = [&]
      {
        auto res = lines[0];

        // Indentation
        static bool const indent = !os::getenv("ELLE_LOG_NO_INDENTATION", false);
        if (indent)
          res = std::string(msg.indentation * 2, ' ') + res;

        // Location
        static bool const location = os::getenv("ELLE_LOG_LOCATIONS", false);
        if (location)
          res = print("%s (at %s:%s in %s)",
                      res, msg.file, msg.line, msg.function);

        // Type
        if (this->_display_type && msg.type != Type::info)
          res = print("[%s] %s", _type_to_string(msg.type), res);

        // Tags
        static bool display_tags = !os::getenv("ELLE_LOG_NO_TAGS", false);
        if (display_tags)
          for (auto const& tag: msg.tags)
            if ((this->_enable_pid || tag.first != "PID")
                && (this->_enable_tid || tag.first != "TID"))
              res = print("[%s] %s", tag.second, res);

        // Component
        static bool show_component = !os::getenv("ELLE_LOG_NO_COMPONENT", false);
        if (show_component)
        {
          auto const size = msg.component.size();
          ELLE_ASSERT_LTE(size, this->component_max_size());
          auto const pad = this->component_max_size() - size;
          res = print("[{}{}{}] {}",
                      std::string(pad / 2, ' '),
                      msg.component,
                      std::string(pad / 2 + pad % 2, ' '),
                      res);
        }

        // Time
        if (this->_enable_time)
          res = print("%s: %s", time, res);

        return res;
      }();

      auto const color_code = get_color_code(msg.level, msg.type);
      this->_output << color_code << first_line << '\n';

      if (lines.size() > 1)
      {
        ELLE_ASSERT_GTE(first_line.size(), lines[0].size());
        auto indent = std::string(first_line.size() - lines[0].size(), ' ');
        for (auto i = 1u; i < lines.size(); i++)
          this->_output << indent << lines[i] << '\n';
      }
      if (!color_code.empty())
        this->_output << "[0m";
      this->_output.flush();
    }
  }
}
