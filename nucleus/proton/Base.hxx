#ifndef NUCLEUS_PROTON_BASE_HXX
# define NUCLEUS_PROTON_BASE_HXX

# include <elle/serialize/Serializer.hh>
# include <elle/cryptography/Digest.hh>
# include <nucleus/proton/Revision.hh>

ELLE_SERIALIZE_SIMPLE(nucleus::proton::Base,
                      archive,
                      value,
                      version)
{
  enforce(version == 0);

  archive & value._revision;
  archive & value._digest;
}

#endif
