#ifndef NUCLEUS_NEUTRON_FELLOW_HXX
# define NUCLEUS_NEUTRON_FELLOW_HXX

# include <cassert>

# include <elle/serialize/ArchiveSerializer.hxx>

ELLE_SERIALIZE_SIMPLE(nucleus::neutron::Fellow,
                      archive,
                      value,
                      version)
{
  assert(version == 0);

  archive & value._subject;
  archive & value._token;
}

#endif
