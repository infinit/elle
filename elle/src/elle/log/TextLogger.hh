#ifndef ELLE_LOG_TEXTLOGGER_HH
# define ELLE_LOG_TEXTLOGGER_HH

# include <iosfwd>

# include <elle/log/Logger.hh>

namespace elle
{
  namespace log
  {
    class TextLogger: public Logger
    {
    public:
      TextLogger(std::ostream& out);
    protected:
      virtual
      void
      _message(Level level,
               elle::log::Logger::Type type,
               std::string const& component,
               std::string const& message,
               std::string const& file,
               unsigned int line,
               std::string const& function);
    private:
      ELLE_ATTRIBUTE_R(std::ostream&, output);
      ELLE_ATTRIBUTE_RW(bool, display_type);
      ELLE_ATTRIBUTE_RW(bool, enable_pid);
      ELLE_ATTRIBUTE_RW(bool, enable_tid);
      ELLE_ATTRIBUTE_RW(bool, enable_time);
      ELLE_ATTRIBUTE_RW(bool, universal_time);
    };
  }
}

#endif
