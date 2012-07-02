#ifndef HOLE_HOLE_HH
# define HOLE_HOLE_HH

# include <elle/types.hh>
# include <elle/concurrency/Signal.hh>
# include <nucleus/Nucleus.hh>

# include <lune/Descriptor.hh>
# include <lune/Set.hh>
# include <lune/Passport.hh>

# include <hole/Holeable.hh>

///
/// this namespace is used by the hole storage abstraction.
///
namespace hole
{

//
// ---------- classes ---------------------------------------------------------
//

  ///
  /// this class represents the abstract Hole component.
  ///
  class Hole
  {
  public:
    //
    // enumerations
    //
    enum State
      {
        StateOffline,
        StateOnline
      };

    //
    // static methods
    //
    static void                 Initialize();
    static elle::Status         Clean();

    static elle::Status         Ready();

    static elle::Status         Origin(nucleus::Address&);
    static elle::Status         Push(const nucleus::Address&,
                                     const nucleus::Block&);
    static elle::Status         Pull(const nucleus::Address&,
                                     const nucleus::Version&,
                                     nucleus::Block&);
    static elle::Status         Wipe(const nucleus::Address&);

    //
    // static attributes
    //
    static lune::Descriptor     Descriptor;
    static lune::Set            Set;
    static lune::Passport       Passport;

    static Holeable*            Implementation;

    // XXX
    static State                state;
    static
    elle::concurrency::Signal<
      elle::radix::Parameters<>
      >                         ready;
  };

}

//
// ---------- includes --------------------------------------------------------
//

#include <hole/Label.hh>
#include <hole/Model.hh>
#include <hole/Manifest.hh>

#include <hole/implementations/Implementations.hh>

#endif
