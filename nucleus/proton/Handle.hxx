#ifndef NUCLEUS_PROTON_HANDLE_HXX
# define NUCLEUS_PROTON_HANDLE_HXX

# include <cryptography/SecretKey.hh>

# include <elle/serialize/Pointer.hh>

ELLE_SERIALIZE_SIMPLE(nucleus::proton::Handle,
                      archive,
                      value,
                      version)
{
  enforce(version == 0);

  archive & value._address;
  archive & elle::serialize::alive_pointer(value._secret);
}

#endif
