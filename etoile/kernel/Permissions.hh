//
// ---------- header ----------------------------------------------------------
//
// project       infinit
//
// license       infinit (c)
//
// file          /home/mycure/infinit/etoile/kernel/Permissions.hh
//
// created       julien quintard   [fri jul 24 16:28:56 2009]
// updated       julien quintard   [sun apr  4 14:40:45 2010]
//

#ifndef ETOILE_KERNEL_RIGHTS_HH
#define ETOILE_KERNEL_RIGHTS_HH

namespace etoile
{
  namespace kernel
  {

//
// ---------- types -----------------------------------------------------------
//

    ///
    /// this enumeration represents the different rights a subject can
    /// be granted.
    ///
    typedef Natural8		Permissions;

//
// ---------- constants -------------------------------------------------------
//

    ///
    /// default permission values.
    ///
    const Permissions		PermissionNone = 0x0;
    const Permissions		PermissionRead = 0x1;
    const Permissions		PermissionWrite = 0x2;
    const Permissions		PermissionReadWrite =
      PermissionRead | PermissionWrite;

  }
}

#endif
