#include <horizon/macosx/Handle.hh>

namespace horizon
{
  namespace macosx
  {

//
// ---------- constructors & destructors --------------------------------------
//

    ///
    /// default constructor.
    ///
    Handle::Handle(const Operation                              operation,
                   const etoile::gear::Identifier&              identifier):
      operation(operation),
      identifier(identifier)
    {
    }

    ///
    /// specific constructor.
    ///
    Handle::Handle(const Operation                              operation,
                   const etoile::gear::Identifier&              identifier,
                   const nucleus::neutron::Permissions& permissions):
      operation(operation),
      identifier(identifier),
      permissions(permissions)
    {
    }

  }
}
