#ifndef ELLE_EXCEPTION_HH
# define ELLE_EXCEPTION_HH

# include <memory>
# include <stdexcept>

# include <elle/attribute.hh>
# include <elle/serialization.hh>
# include <elle/Backtrace.hh>
# include <elle/serialization/fwd.hh>
# include <elle/types.hh>

namespace elle
{
  /// Base class for exception, with backtrace.
  class Exception
    : public std::runtime_error
    , public elle::serialization::VirtuallySerializable
  {
  /*-------------.
  | Construction |
  `-------------*/
  public:
    Exception(std::string const& message, int skip = 0);
    Exception(elle::Backtrace const& bt, std::string const& message);
    ~Exception() noexcept (true);
    void
    inner_exception(std::unique_ptr<Exception>&& exception);

  /*--------------.
  | Serialization |
  `--------------*/
  public:
    typedef elle::serialization_tag serialization_tag;
    Exception(elle::serialization::SerializerIn& input);
    virtual
    void
    serialize(elle::serialization::Serializer& s);

  private:
    ELLE_ATTRIBUTE_R(Backtrace, backtrace);
    ELLE_ATTRIBUTE_R(std::shared_ptr<Exception>, inner_exception);
  };

  std::ostream& operator << (std::ostream& s, Exception const& e);

  // Try to return the most exhaustive string representing the given
  // exception (or the current one if none is given).
  std::string
  exception_string(std::exception_ptr err = std::exception_ptr{});
}

#endif
