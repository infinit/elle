# include <elle/Exception.hh>
# include <iostream>

namespace elle
{
  /*-------------.
  | Construction |
  `-------------*/

  Exception::Exception(elle::String const& message):
    Exception(Backtrace::current(1), message)
  {}

  Exception::Exception(elle::Backtrace const& bt, elle::String const& message):
    std::runtime_error(message),
    _backtrace(bt),
    _inner_exception(0)
  {}

  std::ostream&
  operator << (std::ostream& s, Exception const& e)
  {
    s << e.what() << std::endl;
    s << e.backtrace();
    if (Exception const* inner = e.inner_exception())
      s << std::endl << "Exception was triggerd by: " << *inner;
    return s;
  }
}
