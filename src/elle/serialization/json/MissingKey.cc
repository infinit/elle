#include <elle/printf.hh>
#include <elle/serialization/json/MissingKey.hh>

namespace elle
{
  namespace serialization
  {
    MissingKey::MissingKey(std::string const& field)
      : Error(elle::sprintf("missing mandatory key: \"%s\"", field))
      , _field(field)
    {}
  }
}
