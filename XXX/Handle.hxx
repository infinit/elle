#ifndef  XXX_HANDLESERIALIZER_HXX
# define XXX_HANDLESERIALIZER_HXX

# include <cassert>

# include <elle/cryptography/SecretKey.hh>
# include <nucleus/proton/Address.hh>

# include <XXX/Handle.hh>

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
