#ifndef  NUCLEUS_PROTON_STAMPSERIALIZER_HXX
# define NUCLEUS_PROTON_STAMPSERIALIZER_HXX

# include <cassert>

# include <elle/cryptography/SignatureSerializer.hxx>
# include <nucleus/proton/LocationSerializer.hxx>

# include <nucleus/proton/Stamp.hh>

ELLE_SERIALIZE_SIMPLE(nucleus::proton::Stamp,
                      archive,
                      value,
                      version)
{
  assert(version == 0);

  archive & value.master;
  archive & value.slave;
  archive & value.signature;
}

#endif
