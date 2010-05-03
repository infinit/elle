//
// ---------- header ----------------------------------------------------------
//
// project       infinit
//
// license       infinit
//
// file          /home/mycure/infinit/etoile/kernel/Permissions.hh
//
// created       julien quintard   [fri jul 24 16:28:56 2009]
// updated       julien quintard   [mon may  3 16:42:32 2010]
//

#ifndef ETOILE_KERNEL_RIGHTS_HH
#define ETOILE_KERNEL_RIGHTS_HH

//
// ---------- includes --------------------------------------------------------
//

#include <elle/Elle.hh>

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
    typedef elle::Natural8	Permissions;

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
