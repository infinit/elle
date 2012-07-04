#ifndef ETOILE_PATH_ROUTE_HXX
# define ETOILE_PATH_ROUTE_HXX

# include <cassert>

# include <elle/idiom/Close.hh>
#  include <elle/serialize/VectorSerializer.hxx>
# include <elle/idiom/Open.hh>

ELLE_SERIALIZE_SIMPLE(etoile::path::Route,
                      archive,
                      value,
                      version)
{
  assert(version == 0);

  archive & value.elements;
}

#endif
