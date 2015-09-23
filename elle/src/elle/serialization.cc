#include <elle/elle.hh>
#include <elle/serialization.hh>

namespace elle
{
  elle::Version serialization_tag::version(
    ELLE_MAJOR, ELLE_MINOR, ELLE_SUBMINOR);
}
