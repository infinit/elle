#ifndef  ETOILE_PATH_CHEMIN_HXX
# define ETOILE_PATH_CHEMIN_HXX

# include <cassert>

# include <etoile/path/Route.hh>
# include <etoile/path/Venue.hh>

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
