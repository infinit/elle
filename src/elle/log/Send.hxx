#include <elle/fwd.hh>
#include <elle/print.hh>

namespace elle
{
  namespace log
  {
    namespace detail
    {
      /// Whether we should abort on invalid log messages.
      ELLE_API
      bool
      debug_formats();

      /// Set whether we should abort on invalid log messages.
      ELLE_API
      void
      debug_formats(bool v);

      inline
      Send::Send()
        : _active(false)
      {}

      template <typename... Args>
      Send::Send(Logger::Level level,
                 Logger::Type type,
                 bool indent,
                 std::string const& component,
                 char const* file,
                 unsigned int line,
                 char const* function,
                 char const* fmt,
                 Args&&... args)
        : _active(true)
      {
        try
        {
          this->_send(level, type, indent, component, file, line, function,
                      elle::print(fmt, std::forward<Args>(args)...));
        }
        // Catching ellipsis to avoid header dependencies. AFAICT only
        // elle::print can throw, and it only throws elle::Error.
        catch (...)
        {
          static auto const error = boost::format("%s:%s: invalid log: %s");
          this->_send(Logger::Level::log,
                      Logger::Type::error,
                      false,
                      "elle.log",
                      __FILE__,
                      __LINE__,
                      ELLE_COMPILER_PRETTY_FUNCTION,
                      str(boost::format(error) % file % line % fmt));
          if (debug_formats())
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
