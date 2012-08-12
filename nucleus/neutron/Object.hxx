#ifndef NUCLEUS_NEUTRON_OBJECT_HXX
# define NUCLEUS_NEUTRON_OBJECT_HXX

# include <cassert>
# include <stdexcept>

# include <elle/serialize/ArchiveSerializer.hxx>

ELLE_SERIALIZE_SIMPLE(nucleus::neutron::Object,
                      archive,
                      value,
                      version)
{
  assert(version == 0);

  archive & static_cast<nucleus::proton::ImprintBlock&>(value);

  archive & value.author;

  archive & value.meta.owner.permissions;
  archive & value.meta.owner.token;
  archive & value.meta.genre;
  archive & value.meta.stamp;
  archive & value.meta.attributes;
  archive & value.meta.access;
  archive & value.meta.version;
  archive & value.meta.signature;

  archive & value.data.contents;
  archive & value.data.size;
  archive & value.data.stamp;
  archive & value.data.version;
  archive & value.data.signature;
}

#endif
