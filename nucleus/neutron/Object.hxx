#ifndef NUCLEUS_NEUTRON_OBJECT_HXX
# define NUCLEUS_NEUTRON_OBJECT_HXX

# include <elle/serialize/Serializer.hh>

# include <nucleus/proton/Radix.hh>
# include <nucleus/neutron/Author.hh>

ELLE_SERIALIZE_SIMPLE(nucleus::neutron::Object,
                      archive,
                      value,
                      version)
{
  enforce(version == 0);

  archive & base_class<nucleus::proton::ImprintBlock>(value);

  archive & elle::serialize::alive_pointer(value._author);

  archive & value._meta.owner.permissions;
  archive & value._meta.owner.token;
  archive & value._meta.genre;
  archive & value._meta.modification_timestamp;
  archive & elle::serialize::alive_pointer(value._meta.attributes);
  archive & elle::serialize::alive_pointer(value._meta.access);
  archive & value._meta.revision;
  archive & elle::serialize::alive_pointer(value._meta.signature);

  archive & elle::serialize::alive_pointer(value._data.contents);
  archive & value._data.size;
  archive & value._data.modification_timestamp;
  archive & value._data.revision;
  archive & elle::serialize::alive_pointer(value._data.signature);
}

#endif
