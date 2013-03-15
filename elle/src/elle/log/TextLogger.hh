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
               std::string const& message);
    private:
      ELLE_ATTRIBUTE_R(std::ostream&, output);
    };
  }
}

#endif
