# include "Exception.hh"

# include <elle/assert.hh>
# include <elle/print.hh>

namespace elle
{
  /*-------------.
  | Construction |
  `-------------*/

  Exception::Exception(std::string const& message):
    Exception(Backtrace::current(1), message)
  {}

  Exception::Exception(elle::Backtrace const& bt,
                       std::string const& message):
    std::runtime_error(message),
    _backtrace(bt),
    _inner_exception(nullptr)
  {}

  void
  Exception::inner_exception(std::unique_ptr<Exception>&& exception)
  {
    this->_inner_exception = std::move(exception);
  }

  std::ostream&
  operator <<(std::ostream& s,
              Exception const& e)
  {
    s << e.what() << std::endl;
    s << e.backtrace();
    if (auto const& inner = e.inner_exception())
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
