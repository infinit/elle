#include <elle/fwd.hh>
#include <elle/print.hh>

namespace elle
{
  namespace log
  {
    namespace detail
    {
      ELLE_API
      bool
      debug_formats();

      ELLE_API
      void
      debug_formats(bool v);

      inline
      Send::Send()
        : _active(false)
      {}

      template <typename... Args>
      Send::Send(elle::log::Logger::Level level,
                 elle::log::Logger::Type type,
                 bool indent,
                 std::string const& component,
                 char const* file,
                 unsigned int line,
                 char const* function,
                 char const* fmt,
                 Args&&... args)
        : _active(true)
      {
        bool debug = debug_formats();
        try
        {
          this->_send(level, type, indent, component, file, line, function,
                      elle::print(fmt, std::forward<Args>(args)...));
        }
        // Catching ellipsis to avoid header dependencies. AFAICT only
        // elle::print can throw, and it only throws elle::Error.
        catch (...)
        {
          this->_send(Logger::Level::log,
                      Logger::Type::error,
                      false,
                      "elle.log",
                      __FILE__,
                      __LINE__,
                      ELLE_COMPILER_PRETTY_FUNCTION,
                      elle::print("%s:%s: invalid log: %s", file, line, fmt)
            );
          if (debug)
            throw;
        }
      }

      inline
      Send::operator bool() const
      {
        return false;
      }

      inline
      Send::~Send()
      {
        if (_active)
          this->_unindent();
      }
    }
  }
}
