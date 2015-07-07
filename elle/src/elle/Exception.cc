# include <elle/Exception.hh>

# include <elle/assert.hh>
# include <elle/print.hh>
# include <elle/serialization/Serializer.hh>

namespace elle
{
  /*-------------.
  | Construction |
  `-------------*/

  Exception::Exception(std::string const& message)
    : Exception(Backtrace::current(1), message)
  {}

  Exception::Exception(elle::Backtrace const& bt,
                       std::string const& message)
    : std::runtime_error(message)
    , _backtrace(bt)
    , _inner_exception(nullptr)
  {}

  Exception::~Exception()
  {}

  void
  Exception::inner_exception(std::unique_ptr<Exception>&& exception)
  {
    this->_inner_exception = std::move(exception);
  }

  /*--------------.
  | Serialization |
  `--------------*/

  Exception::Exception(elle::serialization::SerializerIn& input)
    : std::runtime_error(input.deserialize<std::string>("message"))
  {}

  void
  Exception::serialize(elle::serialization::Serializer& s)
  {
    std::string message = this->what();
    s.serialize("message", message);
  }

  /*--------.
  | Helpers |
  `--------*/

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
      return e.what();
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
