#ifndef  NUCLEUS_PROTON_LOCATION_HXX
# define NUCLEUS_PROTON_LOCATION_HXX


# include <elle/serialize/Serializer.hh>

ELLE_SERIALIZE_SIMPLE(nucleus::proton::Location,
                      archive,
                      value,
                      version)
{
  enforce(version == 0);

  archive & value.address;
  archive & value.revision;
}

#endif
