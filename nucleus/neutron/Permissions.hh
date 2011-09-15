//
// ---------- header ----------------------------------------------------------
//
// project       nucleus
//
// license       infinit
//
// author        julien quintard   [fri jul 24 16:28:56 2009]
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
    /// this enumeration represents the different permissions a subject can
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
