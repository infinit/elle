#ifndef  NUCLEUS_PROTON_BLOCK_HXX
# define NUCLEUS_PROTON_BLOCK_HXX

# include <cassert>

# include <elle/serialize/ArchiveSerializer.hxx>

ELLE_SERIALIZE_SIMPLE(nucleus::proton::Block,
                      archive,
                      value,
                      version)
{
  assert(version == 0);
  archive & value.network;
  archive & value.family;
  assert(static_cast<int>(value.family) < static_cast<int>(nucleus::proton::Families));
  archive & value.component;
  assert(static_cast<int>(value.component) < static_cast<int>(nucleus::neutron::Components));
}

#endif
