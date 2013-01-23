#ifndef NUCLEUS_PROTON_STAMP_HXX
# define NUCLEUS_PROTON_STAMP_HXX

# include <elle/serialize/Serializer.hh>
# include <elle/serialize/Pointer.hh>

ELLE_SERIALIZE_SIMPLE(nucleus::proton::Stamp,
                      archive,
                      value,
                      version)
{
  enforce(version == 0);

  archive & value.master;
  archive & value.slave;
  archive & elle::serialize::alive_pointer(value._signature);
}

#endif
