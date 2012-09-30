#ifndef NUCLEUS_NEUTRON_RIGHTS_HH
# define NUCLEUS_NEUTRON_RIGHTS_HH

# include <elle/types.hh>

namespace nucleus
{
  namespace neutron
  {

//
// ---------- enumerations ----------------------------------------------------
//

    /// Defines the permissions a subject (i.e user or group) can be granted
    /// on a file system object.
    ///
    /// Note that for convenience, the write permission implictly includes
    /// the read permission as well.
    enum class Permissions
    {
      none = 0,
      read = (1 << 0),
      write = read | (1 << 1)
    };

  }
}

#endif
