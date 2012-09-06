#ifndef  NUCLEUS_PROTON_VERSION_HXX
# define NUCLEUS_PROTON_VERSION_HXX

# include <elle/serialize/Serializer.hh>

ELLE_SERIALIZE_SIMPLE(nucleus::proton::Version,
                      archive,
                      value,
                      version)
{
  enforce(version == 0);
  archive & value.number;
}

#endif
