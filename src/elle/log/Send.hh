#pragma once

#include <elle/compiler.hh>
#include <elle/log/Logger.hh>
#include <elle/memory.hh>

namespace elle
{
  namespace log
  {
    /// The current logger.
    ELLE_API
    Logger&
    logger();

    /// Change the current logger.
    ///
    /// @return the previous one.
    ELLE_API
    std::unique_ptr<Logger>
    logger(std::unique_ptr<Logger> l);

    /// Create a new logger from a spec string.
    std::unique_ptr<Logger>
    make_logger(std::string const& targets);

    /// Add a new logger.
    void
    logger_add(std::unique_ptr<Logger> l);

    /// Here the simplest type possible is used (.rodata-located) so
    /// as to make sure that its initialization will always take place
    /// before the other global variables whose construction may require
    /// logging.
    namespace detail
    {
      struct ELLE_API Send
      {
      public:
        /// Send a log message.
        ///
        /// @param level   the verbosity level
        /// @param type    the severity
        /// @param indent  whether to indent when nesting
        /// @param component  the name of the component, e.g. "elle.Log"
        /// @param file    the source file from which comes the message
        /// @param line    and its line number
        /// @param function and the name of the calling function
        /// @param fmt     format of the message
        /// @param args    arguments for the format string
        template <typename... Args>
        Send(Logger::Level level,
             Logger::Type type,
             bool indent,
             std::string const& component,
             char const* file,
             unsigned int line,
             char const* function,
             char const* fmt,
             Args&&... args);
        Send(); // no-op sender
        ~Send();
        /// Whether is enabled.
        operator bool() const;

      /*------------.
      | Indentation |
      `------------*/
      protected:
        /// Augment indentation level, and stack this component.
        void
        _indent(const std::string& component);
        void
        _unindent();
      private:
        /// Whether not to be ignored.
        bool _active;
      public: // used by macros
        /// Whether messages of this kind are reported.
        ///
        /// Costly, so cache the result (see macros.hh).
        static bool active(Logger::Level level,
                           Logger::Type type,
                           std::string const& component);

      private:
        void _send(Logger::Level level,
                   Logger::Type type,
                   bool indent,
                   std::string const& component,
                   char const* file,
                   unsigned int line,
                   char const* function,
                   const std::string& msg);
        // Indentation level.
        int* _indentation = nullptr;
      };
    }
  }
}

#include <elle/log/Send.hxx>
