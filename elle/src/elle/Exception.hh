#ifndef ELLE_EXCEPTION_HH
# define ELLE_EXCEPTION_HH

# include <memory>
# include <stdexcept>

# include <elle/Backtrace.hh>
# include <elle/attribute.hh>
# include <elle/compiler.hh>
# include <elle/serialization.hh>
# include <elle/serialization/fwd.hh>
# include <elle/types.hh>

namespace elle
{
  /// Base class for exception, with backtrace.
  class ELLE_API Exception
    : public std::runtime_error
    , public elle::serialization::VirtuallySerializable<true>
  {
  /*-------------.
  | Construction |
  `-------------*/
  public:
    Exception(std::string const& message, int skip = 0);
    Exception(elle::Backtrace const& bt, std::string const& message);
    ~Exception() noexcept (true);
    void
    inner_exception(std::exception_ptr exception);

  /*--------------.
  | Serialization |
  `--------------*/
  public:
    typedef elle::serialization_tag serialization_tag;
    Exception(elle::serialization::SerializerIn& input);
    virtual
    void
    serialize(elle::serialization::Serializer& s,
              elle::Version const& version);

  private:
    template <class T>
    friend
    void
    throw_with_nested(T&& t);
    ELLE_ATTRIBUTE_RW(Backtrace, backtrace);
    ELLE_ATTRIBUTE_R(std::exception_ptr, inner_exception);
  };

  ELLE_API
  std::ostream& operator << (std::ostream& s, Exception const& e);

  template <class T>
  ELLE_API
  ELLE_COMPILER_ATTRIBUTE_NORETURN
  void
  throw_with_nested(T&& t);

  // Try to return the most exhaustive string representing the given
  // exception (or the current one if none is given).
  ELLE_API
  std::string
  exception_string(std::exception_ptr err = std::exception_ptr{});
  ELLE_API
  std::string
  exception_string(std::exception_ptr err, std::exception_ptr cur);
}

# include <elle/Exception.hxx>

#endif
