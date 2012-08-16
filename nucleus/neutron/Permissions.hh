#ifndef NUCLEUS_NEUTRON_RIGHTS_HH
# define NUCLEUS_NEUTRON_RIGHTS_HH

# include <elle/types.hh>

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
    typedef elle::Natural16 Permissions;

//
// ---------- constants -------------------------------------------------------
//

    ///
    /// default permission values.
    ///
    /// note that for convenience, the write permission implictly includes
    /// the read permission as well.
    ///
    const Permissions PermissionNone = 0;
    const Permissions PermissionRead = (1 << 0);
    const Permissions PermissionWrite = PermissionRead | (1 << 1);

  }
}

#endif
