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

    /// Defines the permissions a subject (i.e user or group) can be granted
    /// on a file system object.
    ///
    /// Note that for convenience, the write permission implictly includes
    /// the read permission as well.
    typedef elle::Natural8 Permissions;

//
// ---------- values ----------------------------------------------------------
//

    namespace permissions
    {
      Permissions const none = 0;
      Permissions const read = (1 << 0);
      Permissions const write = read | (1 << 1);
    }

//
// ---------- operators -------------------------------------------------------
//

    std::ostream&
    operator <<(std::ostream& stream,
                Permissions const permissions);

  }
}

#endif
