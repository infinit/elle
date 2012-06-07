#ifndef  NUCLEUS_PROTON_STAMPSERIALIZER_HXX
# define NUCLEUS_PROTON_STAMPSERIALIZER_HXX

# include <cassert>

# include <elle/cryptography/Signature.hh>
# include <nucleus/proton/Location.hh>

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
