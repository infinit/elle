#ifndef HOLE_HOLE_HH
# define HOLE_HOLE_HH

# include <boost/signals.hpp>

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

  /*------.
  | Ready |
  `------*/
  public:
    /// Signal other components the storage layer is ready.
    static void ready();
    static void readyHook(boost::function<void ()> const& f);
  private:
    static boost::signal<void ()> _ready;

    //
    // static methods
    //
  public:
    static void                 Initialize();
    /// XXX
    static
    void
    join();
    /// XXX
    static
    void
    leave();
    static elle::Status         Clean();

    static elle::Status         Origin(nucleus::proton::Address&);
    static elle::Status         Push(const nucleus::proton::Address&,
                                     const nucleus::proton::Block&);
    /// The block associated with the given address.
    static std::unique_ptr<nucleus::proton::Block>
    Pull(const nucleus::proton::Address&, const nucleus::proton::Version&);
    /// Remove the block associated with the given address.
    static void Wipe(const nucleus::proton::Address&);

    //
    // static attributes
    //
    static lune::Descriptor     Descriptor;
    static lune::Set            Set;
    static lune::Passport       Passport;

    static Holeable*            Implementation;

    // XXX
    static State                state;
  };

}

#endif
