#ifndef NUCLEUS_NEUTRON_GROUP_HXX
# define NUCLEUS_NEUTRON_GROUP_HXX

/*-------------.
| Serializable |
`-------------*/

# include <elle/serialize/Serializer.hh>
# include <elle/serialize/Pointer.hh>

# include <nucleus/proton/Radix.hh>

ELLE_SERIALIZE_SIMPLE(nucleus::neutron::Group,
                      archive,
                      value,
                      version)
{
  enforce(version == 0);

  archive & base_class<nucleus::proton::ImprintBlock>(value);

  archive & value._description;
  archive & elle::serialize::alive_pointer(value._pass_K);
  archive & value._size;
  archive & value._modification_timestamp;
  archive & elle::serialize::alive_pointer(value._ensemble);
  archive & value._manager_token;
  archive & elle::serialize::alive_pointer(value._signature);
}

#endif
