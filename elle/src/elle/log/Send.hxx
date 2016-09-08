#ifndef ELLE_LOG_SEND_HXX
# define ELLE_LOG_SEND_HXX

# include <elle/fwd.hh>
# include <elle/print.hh>
# include <elle/printf.hh>

namespace elle
{
  namespace log
  {
    namespace detail
    {
      bool
      debug_formats();
      void
      debug_formats(bool v);

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
        bool debug = debug_formats();
        try
        {
          if (this->_proceed)
            this->_send(level, type, indent, component, file, line, function,
                        elle::sprintf(fmt, std::forward<Args>(args)...));
          else if (debug)
            elle::sprintf(fmt, std::forward<Args>(args)...);
        }
        // Catching ellipsys to avoid header dependencies. AFAICT only
        // elle::print can throw, and it only throw elle::Error.
        catch (...)
        {
          this->_send(Logger::Level::log,
                      Logger::Type::error,
                      false,
                      "elle.log",
                      __FILE__,
                      __LINE__,
                      ELLE_COMPILER_PRETTY_FUNCTION,
                      elle::sprintf("%s:%s: invalid log: %s", file, line, fmt)
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

      template <typename... Args>
      std::string
      print_function_params(Args&&... args)
      {
        return elle::sprint(elle::iomanip::Separator(", "),
                            std::forward<Args>(args)...);
      }
    }
  }
}

#endif
