#ifndef NUCLEUS_PROTON_HANDLE_HXX
# define NUCLEUS_PROTON_HANDLE_HXX

# include <cryptography/SecretKey.hh>

ELLE_SERIALIZE_SIMPLE(nucleus::proton::Handle,
                      archive,
                      value,
                      version)
{
  enforce(version == 0);

  archive & value._address;
  archive & value._secret;
}

#endif
