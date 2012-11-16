#ifndef NUCLEUS_PROTON_PLACEMENT_HXX
# define NUCLEUS_PROTON_PLACEMENT_HXX

# include <elle/serialize/Serializer.hh>

ELLE_SERIALIZE_SIMPLE(nucleus::proton::Placement,
                      archive,
                      value,
                      version)
{
  enforce(version == 0);

  archive & value._number;
}

#endif
