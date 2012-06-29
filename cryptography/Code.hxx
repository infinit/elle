#ifndef  ELLE_CRYPTOGRAPHY_CODE_HXX
# define ELLE_CRYPTOGRAPHY_CODE_HXX

# include <cassert>

# include <elle/serialize/ArchiveSerializer.hxx>
# include <elle/standalone/Region.hh>

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
