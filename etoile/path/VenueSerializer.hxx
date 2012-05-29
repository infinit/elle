#ifndef  ETOILE_PATH_VENUESERIALIZER_HXX
# define ETOILE_PATH_VENUESERIALIZER_HXX

# include <cassert>

# include <elle/serialize/VectorSerializer.hxx>
# include <nucleus/proton/LocationSerializer.hxx>

# include <etoile/path/Venue.hh>

ELLE_SERIALIZE_SIMPLE(etoile::path::Venue,
                      archive,
                      value,
                      version)
{
  assert(version == 0);

  archive & value.elements;
}

#endif
