//
// ---------- header ----------------------------------------------------------
//
// project       nucleus
//
// license       infinit
//
// file          /home/mycure/infinit/nucleus/neutron/Permissions.hh
//
// created       julien quintard   [fri jul 24 16:28:56 2009]
// updated       julien quintard   [thu may  5 09:11:49 2011]
//

#ifndef NUCLEUS_NEUTRON_RIGHTS_HH
#define NUCLEUS_NEUTRON_RIGHTS_HH

//
// ---------- includes --------------------------------------------------------
//

#include <elle/Elle.hh>

namespace nucleus
{
  namespace neutron
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
