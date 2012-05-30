#ifndef  NUCLEUS_PROTON_BASESERIALIZER_HXX
# define NUCLEUS_PROTON_BASESERIALIZER_HXX

# include <cassert>

# include <elle/serialize/ArchiveSerializer.hxx>
# include <elle/cryptography/DigestSerializer.hxx>
# include <nucleus/proton/VersionSerializer.hxx>

# include <nucleus/proton/Base.hh>

ELLE_SERIALIZE_SIMPLE(nucleus::proton::Base,
                      archive,
                      value,
                      version)
{
  assert(version == 0);

  archive & value.version;
  archive & value.digest;
}

#endif
