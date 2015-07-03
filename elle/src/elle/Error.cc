#include <elle/Error.hh>
#include <elle/serialization/Serializer.hh>

namespace elle
{
  /*-------------.
  | Construction |
  `-------------*/

  Error::Error(std::string const& format)
    : Super(format)
  {}

  Error::Error(elle::Backtrace const& bt, std::string const& format)
    : Super(bt, format)
  {}

  /*--------------.
  | Serialization |
  `--------------*/

  Error::Error(elle::serialization::SerializerIn& input)
    : Super(input)
  {}

  static const elle::serialization::Hierarchy<elle::Exception>::
  Register<Error> _register_serialization;
}
