#ifndef  ETOILE_PATH_CHEMINSERIALIZER_HXX
# define ETOILE_PATH_CHEMINSERIALIZER_HXX

# include <cassert>

# include <etoile/path/RouteSerializer.hxx>
# include <etoile/path/VenueSerializer.hxx>

# include <etoile/path/Chemin.hh>

ELLE_SERIALIZE_SIMPLE(etoile::path::Chemin,
                      archive,
                      value,
                      version)
{
  assert(version == 0);

  archive & value.route;
  archive & value.venue;
}

#endif
