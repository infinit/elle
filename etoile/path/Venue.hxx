#ifndef  ETOILE_PATH_VENUE_HXX
# define ETOILE_PATH_VENUE_HXX

# include <cassert>

# include <elle/serialize/VectorSerializer.hxx>
# include <nucleus/proton/Location.hh>

ELLE_SERIALIZE_SIMPLE(etoile::path::Venue,
                      archive,
                      value,
                      version)
{
  assert(version == 0);

  archive & value.elements;
}

#endif
