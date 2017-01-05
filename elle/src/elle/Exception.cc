# include <elle/Exception.hh>

# include <elle/assert.hh>
# include <elle/serialization/Serializer.hh>

namespace elle
{
  /*-------------.
  | Construction |
  `-------------*/

  Exception::Exception(std::string const& message, int skip)
    : Exception(Backtrace::current(1 + skip), message)
  {}

  Exception::Exception(elle::Backtrace const& bt,
                       std::string const& message)
    : std::runtime_error(message)
    , _backtrace(bt)
    , _inner_exception(nullptr)
  {}

  Exception::~Exception() noexcept (true)
  {}

  void
  Exception::inner_exception(std::exception_ptr exception)
  {
    this->_inner_exception = exception;
  }

  /*--------------.
  | Serialization |
  `--------------*/

  Exception::Exception(elle::serialization::SerializerIn& input)
    : std::runtime_error(input.deserialize<std::string>("message"))
  {}

  void
  Exception::serialize(elle::serialization::Serializer& s,
                       elle::Version const& version)
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
    s << e.what();
    return s;
  }

  std::string
  exception_string(std::exception_ptr eptr, std::exception_ptr cur)
  {
    if (!eptr)
    {
      ELLE_ASSERT(cur);
      eptr = cur;
    }
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
