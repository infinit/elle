#ifndef  NUCLEUS_NEUTRON_ENTRY_HXX
# define NUCLEUS_NEUTRON_ENTRY_HXX

# include <cassert>

# include <elle/serialize/ArchiveSerializer.hxx>
# include <nucleus/proton/Address.hh>

# include <nucleus/neutron/Entry.hh>

ELLE_SERIALIZE_SIMPLE(nucleus::neutron::Entry,
                      archive,
                      value,
                      version)
{
  assert(version == 0);

  archive & value.name;
  archive & value.address;
}

#endif
