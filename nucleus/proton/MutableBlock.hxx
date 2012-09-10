#ifndef  NUCLEUS_PROTON_MUTABLE_BLOCK__HXX
# define NUCLEUS_PROTON_MUTABLE_BLOCK__HXX

# include <elle/serialize/Serializer.hh>

ELLE_SERIALIZE_SIMPLE(nucleus::proton::MutableBlock,
                      archive,
                      value,
                      version)
{
  enforce(version == 0);
  archive & base_class<nucleus::proton::Block>(value);
  archive & value.version;
}

#endif
