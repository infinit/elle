#ifndef  ETOILE_PATH_ROUTE_HXX
# define ETOILE_PATH_ROUTE_HXX

# include <cassert>

# include <elle/serialize/VectorSerializer.hxx>

# include <etoile/path/Route.hh>

ELLE_SERIALIZE_SIMPLE(etoile::path::Route,
                      archive,
                      value,
                      version)
{
  assert(version == 0);

  archive & value.elements;
}

#endif
