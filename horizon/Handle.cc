//
// ---------- header ----------------------------------------------------------
//
// project       pig
//
// license       infinit
//
// author        julien.quintard   [tue sep 27 16:47:36 2011]
//

//
// ---------- includes --------------------------------------------------------
//

#include <pig/Handle.hh>

namespace pig
{

//
// ---------- constructors & destructors --------------------------------------
//

  ///
  /// default constructor.
  ///
  Handle::Handle(const Operation				operation,
		 const etoile::gear::Identifier&		identifier):
    operation(operation),
    identifier(identifier)
  {
  }

  ///
  /// specific constructor.
  ///
  Handle::Handle(const Operation				operation,
		 const etoile::gear::Identifier&		identifier,
		 const nucleus::Permissions&			permissions):
    operation(operation),
    identifier(identifier),
    permissions(permissions)
  {
  }

}
