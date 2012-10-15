#ifndef HOLEFACTORY_HH
# define HOLEFACTORY_HH

# include <elle/fwd.hh>

# include <hole/Hole.hh>

namespace infinit
{
  std::unique_ptr<hole::Hole>
  hole_factory(hole::storage::Storage& storage,
               elle::Passport const& passport,
               elle::Authority const& authority);
}

#endif
