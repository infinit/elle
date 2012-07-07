#ifndef HOLE_HOLE_HH
# define HOLE_HOLE_HH

# include <elle/types.hh>
# include <elle/concurrency/Signal.hh>

# include <nucleus/proton/fwd.hh>

# include <lune/Descriptor.hh>
# include <lune/Set.hh>
# include <lune/Passport.hh>

# include <hole/fwd.hh>

///
/// this namespace is used by the hole storage abstraction.
///
namespace hole
{
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

    static elle::Status         Origin(nucleus::proton::Address&);
    static elle::Status         Push(const nucleus::proton::Address&,
                                     const nucleus::proton::Block&);
    static elle::Status         Pull(const nucleus::proton::Address&,
                                     const nucleus::proton::Version&,
                                     nucleus::proton::Block&);
    static elle::Status         Wipe(const nucleus::proton::Address&);

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

#endif
