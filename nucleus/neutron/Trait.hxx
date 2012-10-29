#ifndef NUCLEUS_NEUTRON_TRAIT_HXX
# define NUCLEUS_NEUTRON_TRAIT_HXX

# include <elle/serialize/Serializer.hh>
# include <elle/serialize/Pointer.hh>

ELLE_SERIALIZE_SIMPLE(nucleus::neutron::Trait,
                      archive,
                      value,
                      version)
{
  enforce(version == 0);

  archive & value._type;
  switch (value._type)
    {
      case nucleus::neutron::Trait::Type::null:
        {
          break;
        }
      case nucleus::neutron::Trait::Type::valid:
        {
          archive & elle::serialize::alive_pointer(value._valid);

          break;
        }
    }
}

ELLE_SERIALIZE_SIMPLE(nucleus::neutron::Trait::Valid,
                      archive,
                      value,
                      version)
{
  enforce(version == 0);

  archive & value._name;
  archive & value._value;
}

#endif
