#ifndef  XXX_HANDLESERIALIZER_HXX
# define XXX_HANDLESERIALIZER_HXX

# include <cassert>

# include <elle/cryptography/SecretKeySerializer.hxx>
# include <nucleus/proton/AddressSerializer.hxx>

# include <XXX/Handle.hh>

ELLE_SERIALIZE_SIMPLE(XXX::Handle,
                      archive,
                      value,
                      version)
{
  assert(version == 0);

  archive & value.address;
  archive & value.secret;
}

#endif
