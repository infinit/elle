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
      static elle::Status       Pull(const nucleus::proton::Address&,
                                     const nucleus::proton::Version&,
                                     nucleus::proton::Block&);
      static elle::Status       Wipe(const nucleus::proton::Address&);
    };

  }
}

#endif
