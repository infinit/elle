#ifndef ETOILE_PATH_ROUTE_HXX
# define ETOILE_PATH_ROUTE_HXX

# include <elle/serialize/VectorSerializer.hxx>

ELLE_SERIALIZE_SIMPLE(etoile::path::Route,
                      archive,
                      value,
                      version)
{
  enforce(version == 0);

  archive & value.elements;
}

#endif
