#ifndef  ELLE_LOG_HXX
# define ELLE_LOG_HXX

# include <elle/printf.hh>

namespace elle
{
  namespace log
  {
    namespace detail
    {
      template <typename ... Args>
      inline
      TraceContext::TraceContext(elle::log::Logger::Level level,
                                 elle::log::Logger::Type type,
                                 elle::String const& component,
                                 char const* file,
                                 unsigned int line,
                                 char const* function,
                                 char const* fmt,
                                 Args const& ... args)
        : _proceed(this->_enabled(type, level, component))
      {
        if (!_proceed)
          return;
        this->_indent();
        this->_send(level, type, component, file, line, function,
                    elle::sprintf(fmt, args...));
      }

      inline
      TraceContext::operator bool() const
      {
        return false;
      }
    }

  }
}

#endif
