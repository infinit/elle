#pragma once

#include <elle/compiler.hh>
#include <elle/log/Logger.hh>
#include <elle/memory.hh>

namespace elle
{
  namespace log
  {
    ELLE_API
    Logger&
    logger();
    ELLE_API
    std::unique_ptr<Logger>
    logger(std::unique_ptr<Logger> l);

    /// Here the simplest type possible is used (.rodata-located) so
    /// as to make sure that its initialization will always take place
    /// before the other global variables whose construction may require
    /// logging.
    namespace detail
    {
      struct ELLE_API Send
      {
      public:
        template <typename... Args>
        Send(elle::log::Logger::Level level,
             elle::log::Logger::Type type,
             bool indent,
             std::string const& component,
             char const* file,
             unsigned int line,
             char const* function,
             char const* fmt,
             Args&&... args);
        Send(); // no-op sender
        ~Send();
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
        static bool active(elle::log::Logger::Level level,
                           elle::log::Logger::Type type,
                           std::string const& component);

      private:
        void _send(elle::log::Logger::Level level,
                   elle::log::Logger::Type type,
                   bool indent,
                   std::string const& component,
                   char const* file,
                   unsigned int line,
                   char const* function,
                   const std::string& msg);
        unsigned int* _indentation = nullptr;
      };
    }
  }
}

#include <elle/log/Send.hxx>
