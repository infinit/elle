#ifndef NUCLEUS_PROTON_REVISION_HXX
# define NUCLEUS_PROTON_REVISION_HXX

# include <elle/serialize/Serializer.hh>

ELLE_SERIALIZE_SIMPLE(nucleus::proton::Revision,
                      archive,
                      value,
                      revision)
{
  enforce(revision == 0);
  archive & value.number;
}

#endif
