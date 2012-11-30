#ifndef NUCLEUS_NEUTRON_RIGHTS_HH
# define NUCLEUS_NEUTRON_RIGHTS_HH

# include <elle/types.hh>

namespace nucleus
{
  namespace neutron
  {
    /*------.
    | Types |
    `------*/

    /// Define the permissions a subject (i.e user or group) can be granted
    /// on a file system object.
    ///
    /// Note that for convenience, the write permission implictly includes
    /// the read permission as well.
    typedef elle::Natural8 Permissions;

    /*-------.
    | Values |
    `-------*/

    namespace permissions
    {
      extern Permissions const none;
      extern Permissions const read;
      extern Permissions const write;
    }

    /*----------.
    | Operators |
    `----------*/

    std::ostream&
    operator <<(std::ostream& stream,
                Permissions const permissions);
  }
}

#endif
