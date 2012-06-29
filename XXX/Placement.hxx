#ifndef  XXX_PLACEMENT_HXX
# define XXX_PLACEMENT_HXX

# include <cassert>

# include <elle/serialize/ArchiveSerializer.hxx>

# include <XXX/Placement.hh>

ELLE_SERIALIZE_SIMPLE(nucleus::proton::Placement,
                      archive,
                      value,
                      version)
{
  assert(version == 0);

  archive & value.value;
}

#endif
