#ifndef ELLE_LOG_SEND_HXX
# define ELLE_LOG_SEND_HXX

# include <elle/printf.hh>

namespace elle
{
  namespace log
  {
    namespace detail
    {
      template <typename... Args>
      inline
      Send::Send(elle::log::Logger::Level level,
                 elle::log::Logger::Type type,
                 bool indent,
                 elle::String const& component,
                 char const* file,
                 unsigned int line,
                 char const* function,
                 char const* fmt,
                 Args&&... args):
        _proceed(this->_enabled(type, level, component)),
        _indentation(nullptr)
      {
        if (!_proceed)
          return;
        this->_send(level, type, indent, component, file, line, function,
                    elle::sprintf(fmt, std::forward<Args>(args)...));
      }

      inline
      Send::operator bool() const
      {
        return false;
      }
    }
  }
}

#endif
