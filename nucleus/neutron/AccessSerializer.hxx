#ifndef  NUCLEUS_NEUTRON_ACCESSSERIALIZER_HXX
# define NUCLEUS_NEUTRON_ACCESSSERIALIZER_HXX

# include <cassert>

# include <elle/serialize/ArchiveSerializer.hxx>
# include <nucleus/neutron/RangeSerializer.hxx>
# include <nucleus/neutron/RecordSerializer.hxx>
# include <nucleus/proton/ContentHashBlockSerializer.hxx>

# include <nucleus/neutron/Access.hh>

ELLE_SERIALIZE_SIMPLE(nucleus::neutron::Access,
                      archive,
                      value,
                      version)
{
  assert(version == 0);

  archive & static_cast<nucleus::proton::ContentHashBlock&>(value);
  archive & value.range;
}

#endif
