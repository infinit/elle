#ifndef  NUCLEUS_NEUTRON_REFERENCE_HXX
# define NUCLEUS_NEUTRON_REFERENCE_HXX

# include <cassert>

# include <elle/serialize/ArchiveSerializer.hxx>

# include <nucleus/neutron/Reference.hh>

ELLE_SERIALIZE_SIMPLE(nucleus::neutron::Reference,
                      archive,
                      value,
                      version)
{
  assert(version == 0);

  archive & value.target;
}

#endif
