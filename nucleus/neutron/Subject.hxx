#ifndef  NUCLEUS_NEUTRON_SUBJECT_HXX
# define NUCLEUS_NEUTRON_SUBJECT_HXX

# include <cassert>

# include <elle/cryptography/PublicKey.hh>
# include <elle/serialize/ArchiveSerializer.hxx>
# include <elle/serialize/Pointer.hh>
# include <elle/print.hh>

# include <nucleus/proton/Address.hh>
# include <nucleus/neutron/Subject.hh>


ELLE_SERIALIZE_SIMPLE(nucleus::neutron::Subject,
                      archive,
                      value,
                      version)
{
  assert(version == 0);

  archive & value.type;

  switch (value.type)
    {
    case nucleus::neutron::Subject::TypeUser:
      {
        archive & elle::serialize::alive_pointer(value.user);
        break;
      }
    case nucleus::neutron::Subject::TypeGroup:
      {
        archive & elle::serialize::alive_pointer(value.group);
        break;
      }
    default:
      {
        // XXX
        printf("THROW SUBJECT\n");

        throw std::runtime_error("Unknown subject type: " + elle::sprint((int) value.type));
        break;
      }
    }
}

#endif
