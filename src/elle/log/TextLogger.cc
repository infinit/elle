#include <elle/log/TextLogger.hh>

#include <unistd.h>

#include <array>
//#include <iomanip> // put_time
#include <iostream>

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
                           std::string const& envvar,
                           bool display_type,
                           bool enable_pid,
                           bool enable_tid,
                           bool enable_time,
                           bool universal_time,
                           bool microsec_time,
                           bool warn_err_only)
      : Super{log_level, envvar}
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

        return res;
      }();

      // The prefix related to time.
      auto const time_prefix = [&] () -> std::string
        {
          if (this->_enable_time)
          {
            using namespace std::chrono;
            auto const t = Clock::to_time_t(msg.time);
            auto const tm
              = (this->time_universal() ? std::gmtime : std::localtime)(&t);
            auto&& os = std::ostringstream{};
            // GCC 4.9 does not support put_time.
            // os << std::put_time(tm, "%F %T");
            {
              // strlen("2017-07-31 05:24:53") = 20;
              auto buf = std::array<char, 24>{};
              // Mingw does not support "%F %T".
              std::strftime(buf.data(), buf.size(), "%Y-%m-%d %H:%M:%S", tm);
              os << buf.data();
            }
            if (this->time_microsec())
            {
              auto const ms =
                (time_point_cast<microseconds>(msg.time)
                 - time_point_cast<seconds>(msg.time))
                .count();
              os << '.' << std::setfill('0') << std::setw(6) << ms;
            }
            os << ": ";
            return os.str();
          }
          else
            return ""s;
        }();

      auto const color_code = get_color_code(msg.level, msg.type);
      this->_output << color_code << time_prefix << first_line << '\n';

      if (2 <= lines.size())
      {
        ELLE_ASSERT_GTE(first_line.size(), lines[0].size());
        // We repeat the time on each subsequent line, so that sorting
        // log files on time keeps these lines together.
        auto const indent
          = std::string(first_line.size() - lines[0].size(), ' ');
        for (auto i = 1u; i < lines.size(); i++)
          this->_output << time_prefix << indent << lines[i] << '\n';
      }
      if (!color_code.empty())
        this->_output << "[0m";
      this->_output.flush();
    }
  }
}
