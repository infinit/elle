#ifndef  NUCLEUS_PROTON_STAMP_HXX
# define NUCLEUS_PROTON_STAMP_HXX

# include <cassert>

# include <elle/serialize/Serializer.hh>

ELLE_SERIALIZE_SIMPLE(nucleus::proton::Stamp,
                      archive,
                      value,
                      version)
{
  enforce(version == 0);

  archive & value.master;
  archive & value.slave;
  archive & value.signature;
}

#endif
