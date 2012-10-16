#ifndef  NUCLEUS_NEUTRON_SUBJECT_HXX
# define NUCLEUS_NEUTRON_SUBJECT_HXX

# include <elle/serialize/Serializer.hh>
# include <elle/serialize/Pointer.hh>

# include <elle/cryptography/PublicKey.hh>
# include <nucleus/proton/Address.hh>

ELLE_SERIALIZE_SIMPLE(nucleus::neutron::Subject,
                      archive,
                      value,
                      version)
{
  enforce(version == 0);

  archive & value._type;

  switch (value._type)
    {
    case nucleus::neutron::Subject::TypeUser:
      {
        archive & elle::serialize::alive_pointer(value._user);
        break;
      }
    case nucleus::neutron::Subject::TypeGroup:
      {
        archive & elle::serialize::alive_pointer(value._group);
        break;
      }
    default:
      {
        break;
      }
    }
}

#endif
