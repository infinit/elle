#ifndef NUCLEUS_NEUTRON_FELLOW_HXX
# define NUCLEUS_NEUTRON_FELLOW_HXX


# include <elle/serialize/Serializer.hh>

ELLE_SERIALIZE_SIMPLE(nucleus::neutron::Fellow,
                      archive,
                      value,
                      version)
{
  enforce(version == 0);

  archive & value._type;
  switch (value._type)
    {
    case nucleus::neutron::Fellow::Type::null:
      {
        break;
      }
    case nucleus::neutron::Fellow::Type::valid:
      {
        archive & elle::serialize::alive_pointer(value._valid);

        break;
      }
    default:
      throw Exception("unknown fellow type '%s'", value._type);
    }
}

ELLE_SERIALIZE_SIMPLE(nucleus::neutron::Fellow::Valid,
                      archive,
                      value,
                      version)
{
  enforce(version == 0);

  archive & value._subject;
  archive & elle::serialize::pointer(value._token);
}

#endif
