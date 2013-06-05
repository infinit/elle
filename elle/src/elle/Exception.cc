# include "Exception.hh"

# include <elle/assert.hh>
# include <elle/print.hh>

namespace elle
{
  /*-------------.
  | Construction |
  `-------------*/

  Exception::Exception(elle::String const& message):
    Exception(Backtrace::current(1), message)
  {}

  Exception::Exception(elle::Backtrace const& bt,
                       elle::String const& message):
    std::runtime_error(message),
    _backtrace(bt),
    _inner_exception(0)
  {}

  std::ostream&
  operator <<(std::ostream& s,
              Exception const& e)
  {
    s << e.what() << std::endl;
    s << e.backtrace();
    if (Exception const* inner = e.inner_exception())
      s << std::endl << "Exception was triggered by: " << *inner;
    return s;
  }

  std::string
  exception_string(std::exception_ptr eptr)
  {
    if (!eptr)
      eptr = std::current_exception();
    if (!eptr)
      throw Exception{"no current exception present"};
    try
    {
      std::rethrow_exception(eptr);
    }
    catch (Exception const& e)
    {
      return elle::sprint(e);
    }
    catch (std::exception const& e)
    {
      return std::string(e.what());
    }
    catch (...)
    {
      return "unknown exception type";
    }

    elle::unreachable();
  }
}
