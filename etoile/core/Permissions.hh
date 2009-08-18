//
// ---------- header ----------------------------------------------------------
//
// project       infinit
//
// license       infinit (c)
//
// file          /home/mycure/infinit/etoile/core/Permissions.hh
//
// created       julien quintard   [fri jul 24 16:28:56 2009]
// updated       julien quintard   [sat jul 25 04:29:06 2009]
//

#ifndef ETOILE_CORE_RIGHTS_HH
#define ETOILE_CORE_RIGHTS_HH

namespace etoile
{
  namespace core
  {

//
// ---------- enumerations ----------------------------------------------------
//

    ///
    /// this enumeration represents the different rights a subject can
    /// be granted.
    ///
    enum Permissions
      {
	PermissionNone = 0,
	PermissionRead = 1,
	PermissionWrite = 2,
	PermissionReadWrite = 3
      };

  }
}

#endif
