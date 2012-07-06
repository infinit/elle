#ifndef  XXX_HANDLE_HXX
# define XXX_HANDLE_HXX

# include <cassert>

# include <elle/cryptography/SecretKey.hh>
# include <nucleus/proton/Address.hh>

ELLE_SERIALIZE_SIMPLE(nucleus::proton::Handle,
                      archive,
                      value,
                      version)
{
  assert(version == 0);

  archive & value.address;
  archive & value.secret;
}

#endif
