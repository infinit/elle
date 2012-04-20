#ifndef  ELLE_CRYPTOGRAPHY_SIGNATURESERIALIZER_HXX
# define ELLE_CRYPTOGRAPHY_SIGNATURESERIALIZER_HXX

# include <cassert>

# include <elle/serialize/ArchiveSerializer.hxx>
# include <elle/standalone/RegionSerializer.hxx>

# include <elle/cryptography/Signature.hh>

ELLE_SERIALIZE_SIMPLE(elle::cryptography::Signature,
                      archive,
                      value,
                      version)
{
  assert(version == 0);
  archive & value.region;
}

#endif
