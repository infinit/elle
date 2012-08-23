#ifndef ETOILE_DEPOT_DEPOT_HH
# define ETOILE_DEPOT_DEPOT_HH

# include <memory>

# include <elle/types.hh>

# include <nucleus/neutron/Object.hh>
# include <nucleus/proton/fwd.hh>

namespace etoile
{
  ///
  /// this namespace contains everything related to the storage layer
  /// abstraction especially through the communication with the Hole
  /// component.
  ///
  namespace depot
  {

    ///
    /// this class abstracts the storage layer.
    ///
    class Depot
    {
    public:
      //
      // methods
      //
      static elle::Status       Origin(nucleus::proton::Address&);

      static elle::Status       Push(const nucleus::proton::Address&,
                                     const nucleus::proton::Block&);

      /// XXX
      static std::unique_ptr<nucleus::neutron::Object>
      pull_object(nucleus::proton::Address const& address,
                  nucleus::proton::Version const & version);
      /// XXX
      static std::unique_ptr<nucleus::neutron::Access>
      pull_access(nucleus::proton::Address const& address);
      /// XXX
      static std::unique_ptr<nucleus::neutron::Group>
      pull_group(nucleus::proton::Address const& address,
                 nucleus::proton::Version const& version);
      /// XXX
      static std::unique_ptr<nucleus::neutron::Ensemble>
      pull_ensemble(nucleus::proton::Address const& address);

      /// XXX
      template <typename T>
      static std::unique_ptr<T>
      pull(nucleus::proton::Address const& address,
           nucleus::proton::Version const& version =
           nucleus::proton::Version::Last);

      static elle::Status       Wipe(const nucleus::proton::Address&);
    };

  }
}

# include <etoile/depot/Depot.hxx>

#endif
