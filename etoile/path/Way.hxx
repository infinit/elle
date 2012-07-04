#ifndef  ETOILE_PATH_WAY_HXX
# define ETOILE_PATH_WAY_HXX

# include <cassert>

# include <elle/serialize/ArchiveSerializer.hxx>

ELLE_SERIALIZE_SIMPLE(etoile::path::Way,
                      archive,
                      value,
                      version)
{
  assert(version == 0);

  archive & value.path;
}

#endif
