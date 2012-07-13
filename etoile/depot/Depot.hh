#ifndef ETOILE_DEPOT_DEPOT_HH
# define ETOILE_DEPOT_DEPOT_HH

# include <elle/types.hh>
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
      std::unique_ptr<nucleus::neutron::Object>
      Depot::pull_object(nucleus::proton::Address const& address,
                         nucleus::proton::Version const & version);

      /// XXX
      std::unique_ptr<nucleus::neutron::Access>
      Depot::pull_access(nucleus::proton::Address const& address);

      /// XXX
      template <typename T>
      std::unique_ptr<T>
      Depot::pull(nucleus::proton::Address const& address,
                  nucleus::proton::Version const& version =
                    nucleus::proton::Version::Latests);

      static elle::Status       Wipe(const nucleus::proton::Address&);
    };

  }
}

# include <etoile/depot/Depot.hxx>

#endif
