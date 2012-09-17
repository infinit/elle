#include <horizon/Handle.hh>

namespace horizon
{
  Handle::Handle(const Operation                              operation,
                 const etoile::gear::Identifier&              identifier):
    operation(operation),
    identifier(identifier)
  {}

  Handle::Handle(const Operation                              operation,
                 const etoile::gear::Identifier&              identifier,
                 const nucleus::neutron::Permissions& permissions):
    operation(operation),
    identifier(identifier),
    permissions(permissions)
  {}
}
