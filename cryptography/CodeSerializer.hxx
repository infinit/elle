#ifndef  ELLE_CRYPTOGRAPHY_CODESERIALIZER_HXX
# define ELLE_CRYPTOGRAPHY_CODESERIALIZER_HXX

# include <cassert>

# include <elle/serialize/ArchiveSerializer.hxx>
# include <elle/standalone/RegionSerializer.hxx>

# include <elle/cryptography/Code.hh>

ELLE_SERIALIZE_SIMPLE(elle::cryptography::Code,
                      archive,
                      value,
                      version)
{
  assert(version == 0);
  archive & value.region;
}

#endif
