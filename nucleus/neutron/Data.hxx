#ifndef NUCLEUS_NEUTRON_DATA_HXX
# define NUCLEUS_NEUTRON_DATA_HXX

# include <cassert>

# include <elle/serialize/ArchiveSerializer.hxx>

ELLE_SERIALIZE_SIMPLE(nucleus::neutron::Data,
                      archive,
                      value,
                      version)
{
  enforce(version == 0);

  archive & value.region;
}

#endif
