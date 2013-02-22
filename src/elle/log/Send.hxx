#ifndef ELLE_LOG_SEND_HXX
# define ELLE_LOG_SEND_HXX

# include <elle/printf.hh>

namespace elle
{
  namespace log
  {
    namespace detail
    {
      template <typename ... Args>
      inline
      Send::Send(elle::log::Logger::Level level,
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
        this->_send(level, type, component, file, line, function,
                    elle::sprintf(fmt, args...));
      }

      inline
      Send::operator bool() const
      {
        return false;
      }

      template <typename... Args>
      std::string
      print_function_params(Args const&... args)
      {
        return elle::sprint(elle::iomanip::Separator(", "), args...);
      }
    }
  }
}

#endif
