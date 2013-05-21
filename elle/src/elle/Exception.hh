#ifndef ELLE_EXCEPTION_HH
# define ELLE_EXCEPTION_HH

# include <stdexcept>

# include <elle/attribute.hh>
# include <elle/Backtrace.hh>
# include <elle/types.hh>

namespace elle
{
  /// Base class for exception, with backtrace.
  class Exception: public std::runtime_error
  {
  /*-------------.
  | Construction |
  `-------------*/
  public:
    Exception(elle::String const& format);
    Exception(elle::Backtrace const& bt, elle::String const& format);

  private:
    ELLE_ATTRIBUTE_R(Backtrace, backtrace);
    ELLE_ATTRIBUTE_RW(Exception*, inner_exception);
  };

  std::ostream& operator << (std::ostream& s, Exception const& e);
}

#endif
