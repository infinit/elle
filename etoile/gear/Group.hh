#ifndef ETOILE_GEAR_GROUP_HH
# define ETOILE_GEAR_GROUP_HH

# include <elle/types.hh>

# include <nucleus/proton/fwd.hh>
# include <nucleus/proton/Location.hh>
# include <nucleus/proton/Limits.hh>
# include <nucleus/neutron/fwd.hh>

# include <etoile/gear/Context.hh>
# include <etoile/gear/Nature.hh>

# include <etoile/nest/fwd.hh>

# include <etoile/automaton/Group.hh>

# include <etoile/wall/Group.hh>

namespace etoile
{
  namespace gear
  {

    /// XXX
    class Group:
      public Context
    {
    public:
      //
      // constants
      //
      static const Nature                       N = NatureGroup;

      //
      // types
      //
      typedef wall::Group                      W;
      typedef automaton::Group                 A;

      typedef nucleus::neutron::Group::Role Role;

      //
      // constructors & destructors
      //
      Group();
      ~Group();

      //
      // interfaces
      //

      // dumpable
      elle::Status      Dump(const elle::Natural32 = 0) const;

      //
      // attributes
      //
      nucleus::proton::Location location;

      std::unique_ptr<nucleus::neutron::Group> group;

      nucleus::proton::Porcupine<nucleus::neutron::Ensemble>* ensemble_porcupine;
      etoile::nest::Nest* ensemble_nest;
      nucleus::proton::Limits ensemble_limits;

      struct
      {
        nucleus::neutron::Group::Role role;
      } rights;
    };

  }
}

#endif
