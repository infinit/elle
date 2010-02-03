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
// updated       julien quintard   [mon feb  1 22:28:02 2010]
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
	PermissionReadWrite = PermissionRead | PermissionWrite
      };

  }
}

#endif
